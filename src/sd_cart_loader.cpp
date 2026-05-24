#ifndef SD_CART_LOADER_CPP
#define SD_CART_LOADER_CPP

// SD-card cart loader for Lilka. See sd_cart_loader.h for behavior.

#include "sd_cart_loader.h"
#include "p8_text_parser.cpp"

#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include <string.h>
#include <stdio.h>

// ---- Internal storage -------------------------------------------------------

// Display entries shown in the picker. We keep the filename next to
// each entry so we can load it later.
struct SDCartSlot {
    char     filename[96];   // absolute path on SD
    char     display[32];    // basename minus .p8, NUL-terminated
};

static SDCartSlot s_slots[PICO8_SD_MAX_CARTS];
static GameCart   s_carts[PICO8_SD_MAX_CARTS];
static uint8_t    s_count = 0;
static LoadedCart s_selected;        // owns buffers for the loaded cart
static int8_t     s_selected_index = -1;

// ---- Helpers ----------------------------------------------------------------

static bool ends_with_p8(const char* name) {
    size_t n = strlen(name);
    if (n < 3) return false;
    const char* ext = name + n - 3;
    return (ext[0] == '.') &&
           (ext[1] == 'p' || ext[1] == 'P') &&
           (ext[2] == '8');
}

static void basename_no_ext(const char* path, char* out, size_t out_sz) {
    const char* base = strrchr(path, '/');
    base = base ? base + 1 : path;
    size_t n = strlen(base);
    if (n >= 3 && (base[n - 3] == '.') &&
        (base[n - 2] == 'p' || base[n - 2] == 'P') &&
        (base[n - 1] == '8')) {
        n -= 3;
    }
    if (n >= out_sz) n = out_sz - 1;
    memcpy(out, base, n);
    out[n] = 0;
}

// ---- Public API -------------------------------------------------------------

uint8_t sd_scan_carts(void) {
    s_count = 0;

    // ``lilka::begin()`` mounts the SD card. If it failed (no card)
    // ``SD.open()`` will return an invalid file and we just report 0.
    File root = SD.open(PICO8_SD_DIR);
    if (!root) {
        Serial.printf("[sd_cart] '%s' not found on SD\n", PICO8_SD_DIR);
        return 0;
    }
    if (!root.isDirectory()) {
        Serial.printf("[sd_cart] '%s' is not a directory\n", PICO8_SD_DIR);
        root.close();
        return 0;
    }

    while (s_count < PICO8_SD_MAX_CARTS) {
        File f = root.openNextFile();
        if (!f) break;
        if (f.isDirectory()) { f.close(); continue; }

        const char* name = f.name();
        if (!ends_with_p8(name)) { f.close(); continue; }

        SDCartSlot& slot = s_slots[s_count];
        // ``File::name()`` on ESP32 Arduino returns either the basename
        // or a path starting with '/'; normalise to an absolute path.
        if (name[0] == '/') {
            snprintf(slot.filename, sizeof(slot.filename), "%s", name);
        } else {
            snprintf(slot.filename, sizeof(slot.filename), "%s/%s", PICO8_SD_DIR, name);
        }
        basename_no_ext(slot.filename, slot.display, sizeof(slot.display));

        // Pre-fill the menu entry (only name is needed for the picker).
        memset(&s_carts[s_count], 0, sizeof(GameCart));
        s_carts[s_count].name     = slot.display;
        s_carts[s_count].name_len = (uint8_t)strlen(slot.display);

        Serial.printf("[sd_cart] found: %s\n", slot.filename);
        s_count++;
        f.close();
    }
    root.close();

    Serial.printf("[sd_cart] %u cart(s) found in %s\n",
                  (unsigned)s_count, PICO8_SD_DIR);
    return s_count;
}

GameCart* sd_get_cart_array(void) { return s_count ? s_carts : nullptr; }
uint8_t   sd_get_cart_count(void) { return s_count; }

bool sd_load_selected_cart(uint8_t index) {
    if (index >= s_count) return false;

    // Free any previously-loaded cart's buffers.
    if (s_selected_index >= 0) {
        p8_cart_free(&s_selected);
        s_selected_index = -1;
    }

    const char* path = s_slots[index].filename;
    Serial.printf("[sd_cart] loading %s\n", path);

    File f = SD.open(path, FILE_READ);
    if (!f) {
        Serial.printf("[sd_cart] failed to open %s\n", path);
        return false;
    }

    size_t sz = f.size();
    if (sz == 0 || sz > 256 * 1024) {
        Serial.printf("[sd_cart] suspicious size %u for %s\n", (unsigned)sz, path);
        f.close();
        return false;
    }

    // Read the whole file into a temporary buffer (in PSRAM if possible).
#if defined(LILKA_BACKEND) || defined(ESP_BACKEND)
    char* buf = (char*)heap_caps_malloc(sz + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) buf = (char*)malloc(sz + 1);
#else
    char* buf = (char*)malloc(sz + 1);
#endif
    if (!buf) {
        Serial.println("[sd_cart] OOM reading cart");
        f.close();
        return false;
    }
    size_t got = f.read((uint8_t*)buf, sz);
    f.close();
    buf[got] = 0;

    bool ok = p8_parse_text(buf, got, s_slots[index].display, &s_selected);

#if defined(LILKA_BACKEND) || defined(ESP_BACKEND)
    heap_caps_free(buf);
#else
    free(buf);
#endif

    if (!ok) {
        Serial.printf("[sd_cart] parse failed for %s\n", path);
        return false;
    }

    // Patch the menu entry so callers (cartParser / init_lua) can use it.
    s_carts[index] = s_selected.cart;
    s_carts[index].name     = s_slots[index].display;
    s_carts[index].name_len = (uint8_t)strlen(s_slots[index].display);
    s_selected_index = (int8_t)index;

    Serial.printf("[sd_cart] loaded %s (code=%u gfx=%u map=%u sfx=%u)\n",
                  s_slots[index].display,
                  (unsigned)s_carts[index].code_len,
                  (unsigned)s_carts[index].gfx_len,
                  (unsigned)s_carts[index].map_len,
                  (unsigned)s_carts[index].sfx_len);
    return true;
}

#endif // SD_CART_LOADER_CPP
