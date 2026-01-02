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

// Lilka display is 280x240, PICO-8 is 128x128
// We'll scale and center the display
#define LILKA_WIDTH  280
#define LILKA_HEIGHT 240

// Calculate scaling and offset for centering
// Using integer scaling factor of 1 (128x128 fits in 240x240 with room)
// Or we can scale up to fill more of the screen
#define SCALE_FACTOR 1  // Can be 1 or 2 (2 would be 256x256, too big)
#define OFFSET_X ((LILKA_WIDTH - SCREEN_WIDTH * SCALE_FACTOR) / 2)
#define OFFSET_Y ((LILKA_HEIGHT - SCREEN_HEIGHT * SCALE_FACTOR) / 2)

// Frame buffer for the scaled output
static lilka::Canvas* canvas = nullptr;

// Previous button state for edge detection
static uint8_t buttons_prev[6] = {0, 0, 0, 0, 0, 0};

// Audio task handle
static TaskHandle_t audioTaskHandle = nullptr;

// HUD buffer (if using HUD feature)
extern uint8_t hud_buffer[];

bool init_platform() {
    // Lilka initialization is handled in main setup()
    return true;
}

bool init_video() {
    // Create canvas for double buffering
    canvas = new lilka::Canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!canvas) {
        Serial.println("Failed to create canvas");
        return false;
    }
    
    // Clear the display
    lilka::display.fillScreen(lilka::colors::Black);
    
    Serial.println("Video initialized for Lilka");
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
    
    // Convert PICO-8 framebuffer to Lilka canvas
    uint16_t* fb = canvas->getFramebuffer();
    
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            palidx_t p = get_pixel(x, y);
            color_t c = palette[p];
            fb[y * SCREEN_WIDTH + x] = c;
        }
    }
    
    // Draw the canvas to the display, centered
    lilka::display.draw16bitRGBBitmap(
        OFFSET_X, OFFSET_Y,
        canvas->getFramebuffer(),
        SCREEN_WIDTH, SCREEN_HEIGHT
    );
}

void draw_hud() {
#ifdef HUD_HEIGHT
    // Draw HUD at bottom of screen
    // Convert HUD buffer and draw it
    for (uint8_t y = 0; y < HUD_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t idx = y * SCREEN_WIDTH + x;
            uint8_t p = hud_buffer[idx];
            color_t c = palette[p];
            lilka::display.drawPixel(
                OFFSET_X + x,
                OFFSET_Y + SCREEN_HEIGHT + y,
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
    
    // Map Lilka buttons to PICO-8 buttons
    // PICO-8: Left, Right, Up, Down, A (O/Z), B (X/X)
    buttons[BTN_IDX_LEFT]  = state.left.pressed ? 1 : 0;
    buttons[BTN_IDX_RIGHT] = state.right.pressed ? 1 : 0;
    buttons[BTN_IDX_UP]    = state.up.pressed ? 1 : 0;
    buttons[BTN_IDX_DOWN]  = state.down.pressed ? 1 : 0;
    buttons[BTN_IDX_A]     = state.a.pressed ? 1 : 0;  // Lilka A -> PICO-8 O/Z
    buttons[BTN_IDX_B]     = state.b.pressed ? 1 : 0;  // Lilka B -> PICO-8 X
    
    // Calculate "just pressed" for menu navigation
    buttons_frame[BTN_IDX_LEFT]  = state.left.justPressed ? 1 : 0;
    buttons_frame[BTN_IDX_RIGHT] = state.right.justPressed ? 1 : 0;
    buttons_frame[BTN_IDX_UP]    = state.up.justPressed ? 1 : 0;
    buttons_frame[BTN_IDX_DOWN]  = state.down.justPressed ? 1 : 0;
    buttons_frame[BTN_IDX_A]     = state.a.justPressed ? 1 : 0;
    buttons_frame[BTN_IDX_B]     = state.b.justPressed ? 1 : 0;
    
    return false; // Don't quit
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
