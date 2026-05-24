/**
 * Lilka Backend for PicoPico PICO-8 Emulator
 * 
 * This backend targets the Lilka DIY handheld console based on ESP32-S3.
 * Hardware specs:
 * - ESP32-S3-WROOM-1-N16R8 (240MHz dual-core, 16MB Flash, 8MB PSRAM)
 * - ST7789 Display (280x240 pixels, 16-bit color)
 * - I2S Audio
 * - 10 buttons (D-pad, A, B, C, D, Start, Select)
 * 
 * References:
 * - https://docs.lilka.dev/
 * - https://github.com/lilka-dev/sdk
 */

#include <lilka.h>
#include "data.h"
#include "engine.c"
#include "sd_cart_loader.cpp"

// Lilka display is 280x240. PICO-8 native res is 128x128.
// We upscale to a 240x240 square (centered horizontally, top-aligned)
// using nearest-neighbor sampling.
#define LILKA_WIDTH   280
#define LILKA_HEIGHT  240

#define PICO_OUT_W    240
#define PICO_OUT_H    240
#define OFFSET_X      ((LILKA_WIDTH  - PICO_OUT_W) / 2)   // 20
#define OFFSET_Y      ((LILKA_HEIGHT - PICO_OUT_H) / 2)   // 0

// Pre-computed source-pixel lookup tables (filled in init_video()).
static uint8_t xmap[PICO_OUT_W];
static uint8_t ymap[PICO_OUT_H];

// Frame buffer for the scaled output (allocated in PSRAM via Canvas)
static lilka::Canvas* canvas = nullptr;

// Previous button state for edge detection
static uint8_t buttons_prev[6] = {0, 0, 0, 0, 0, 0};

// Audio task handle
static TaskHandle_t audioTaskHandle = nullptr;

// HUD buffer (if using HUD feature)
extern uint8_t hud_buffer[];

// Forward decls of the overridable cart-list globals defined in main.cpp
// (this file is #included into the same translation unit).
extern GameCart* active_carts;
extern uint8_t   active_carts_count;
extern bool    (*pico_prepare_cart)(uint8_t index);

static bool lilka_prepare_cart(uint8_t index) {
    return sd_load_selected_cart(index);
}

bool init_platform() {
    // Scan the SD card for .p8 files in /Pico8/. If we find any, use
    // them as the active cart list; otherwise fall back to the carts
    // baked into the firmware so the device is still usable.
    uint8_t n = sd_scan_carts();
    if (n > 0) {
        active_carts       = sd_get_cart_array();
        active_carts_count = n;
        pico_prepare_cart  = lilka_prepare_cart;
        Serial.printf("[lilka] using %u SD cart(s) from %s\n",
                      (unsigned)n, PICO8_SD_DIR);
    } else {
        Serial.println("[lilka] no SD carts found, using built-in carts");
    }
    return true;
}

bool init_video() {
    // Allocate the upscaled output canvas (240*240*2 = 112.5 KiB).
    // lilka::Canvas allocates in PSRAM on Lilka v2.
    canvas = new lilka::Canvas(PICO_OUT_W, PICO_OUT_H);
    if (!canvas) {
        Serial.println("Failed to create canvas");
        return false;
    }

    // Build nearest-neighbor lookup tables once.
    for (int i = 0; i < PICO_OUT_W; ++i) {
        xmap[i] = (uint8_t)((i * SCREEN_WIDTH)  / PICO_OUT_W);
    }
    for (int i = 0; i < PICO_OUT_H; ++i) {
        ymap[i] = (uint8_t)((i * SCREEN_HEIGHT) / PICO_OUT_H);
    }

    // Clear the display
    lilka::display.fillScreen(lilka::colors::Black);

    Serial.println("Video initialized for Lilka (240x240 upscaled)");
    return true;
}

void video_close() {
    if (canvas) {
        delete canvas;
        canvas = nullptr;
    }
}

void gfx_flip() {
    if (!canvas) return;

    uint16_t* fb = canvas->getFramebuffer();

    // Nearest-neighbor upscale 128x128 -> 240x240.
    for (uint16_t dy = 0; dy < PICO_OUT_H; ++dy) {
        const uint8_t sy = ymap[dy];
        uint16_t* row = fb + (uint32_t)dy * PICO_OUT_W;
        for (uint16_t dx = 0; dx < PICO_OUT_W; ++dx) {
            palidx_t p = get_pixel(xmap[dx], sy);
            row[dx] = palette[p];
        }
    }

    lilka::display.draw16bitRGBBitmap(
        OFFSET_X, OFFSET_Y,
        fb,
        PICO_OUT_W, PICO_OUT_H
    );
}

void draw_hud() {
#ifdef HUD_HEIGHT
    // HUD is a SCREEN_WIDTH x HUD_HEIGHT strip; stretch it across the
    // bottom of the 240x240 output area using the same nearest-neighbor
    // scheme. Width scales horizontally to PICO_OUT_W.
    const uint16_t hud_dst_h = (uint16_t)((HUD_HEIGHT * PICO_OUT_H) / SCREEN_HEIGHT);
    if (hud_dst_h == 0) return;
    for (uint16_t dy = 0; dy < hud_dst_h; ++dy) {
        const uint8_t sy = (uint8_t)((dy * HUD_HEIGHT) / hud_dst_h);
        for (uint16_t dx = 0; dx < PICO_OUT_W; ++dx) {
            const uint8_t sx = xmap[dx];
            const uint8_t p = hud_buffer[sy * SCREEN_WIDTH + sx];
            const color_t c = palette[p];
            lilka::display.drawPixel(
                OFFSET_X + dx,
                OFFSET_Y + PICO_OUT_H - hud_dst_h + dy,
                c
            );
        }
    }
#endif
}

// Rename to avoid conflict with Arduino's delay()
// Use a different name entirely to avoid any ambiguity
void pico_delay_ms(uint16_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

// Macro to redirect all delay() calls to our function in this translation unit
#define delay(x) pico_delay_ms(x)

bool handle_input() {
    // Get current button state from Lilka controller
    lilka::State state = lilka::controller.getState();
    
    // Save previous state for edge detection
    for (int i = 0; i < 6; i++) {
        buttons_prev[i] = buttons[i];
    }
    
    // Map Lilka buttons to PICO-8 buttons.
    // PICO-8 P1 has 6 buttons: Left, Right, Up, Down, O (4), X (5).
    // Lilka has UP/DOWN/LEFT/RIGHT, A, B, C, D, SELECT, START.
    // A and C both act as PICO-8 O (button 4).
    // B and D both act as PICO-8 X (button 5).
    // START or SELECT exits the cart back to the picker.
    buttons[BTN_IDX_LEFT]  = state.left.pressed  ? 1 : 0;
    buttons[BTN_IDX_RIGHT] = state.right.pressed ? 1 : 0;
    buttons[BTN_IDX_UP]    = state.up.pressed    ? 1 : 0;
    buttons[BTN_IDX_DOWN]  = state.down.pressed  ? 1 : 0;
    buttons[BTN_IDX_A]     = (state.a.pressed || state.c.pressed) ? 1 : 0;
    buttons[BTN_IDX_B]     = (state.b.pressed || state.d.pressed) ? 1 : 0;

    // "Just pressed" flags for btnp() and menu navigation.
    buttons_frame[BTN_IDX_LEFT]  = state.left.justPressed  ? 1 : 0;
    buttons_frame[BTN_IDX_RIGHT] = state.right.justPressed ? 1 : 0;
    buttons_frame[BTN_IDX_UP]    = state.up.justPressed    ? 1 : 0;
    buttons_frame[BTN_IDX_DOWN]  = state.down.justPressed  ? 1 : 0;
    buttons_frame[BTN_IDX_A]     = (state.a.justPressed || state.c.justPressed) ? 1 : 0;
    buttons_frame[BTN_IDX_B]     = (state.b.justPressed || state.d.justPressed) ? 1 : 0;

    // START / SELECT: quit cart -> back to picker (via reboot for a clean reset).
    // Skip the first few polls: the controller task initialisation right after
    // lilka::begin() can emit a spurious justPressed on its first getState().
    static uint8_t input_warmup = 30; // ~1 second at 30 FPS
    if (input_warmup > 0) {
        input_warmup--;
    } else if (state.start.justPressed || state.select.justPressed) {
        return true; // signals engine to set wants_to_quit
    }

    return false;
}

uint32_t now() {
    return millis();
}

bool init_audio() {
    // Lilka v2 has I2S audio output
    // For simplicity, we can start with buzzer-based audio
    // or implement full I2S support later
    
    Serial.println("Audio initialized (using Lilka I2S)");
    
    return true;
}

// Time functions for HUD display
uint8_t current_hour() {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo->tm_hour;
}

uint8_t current_minute() {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo->tm_min;
}

uint8_t wifi_strength() {
    // Return 0-3 scale
    // Lilka doesn't have a direct API for this, but we can check WiFi
    // For now, return 0 (no WiFi indicator)
    return 0;
}

uint8_t battery_left() {
    // Use Lilka battery API
    int level = lilka::battery.readLevel();
    
    // Convert percentage to 0-3 scale
    if (level > 75) return 3;
    if (level > 50) return 2;
    if (level > 25) return 1;
    return 0;
}
