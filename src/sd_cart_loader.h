#ifndef SD_CART_LOADER_H
#define SD_CART_LOADER_H

// SD-card cart scanning + loading for the Lilka backend.
//
// Carts are expected as ``.p8`` text files in ``/sd/Pico8/`` on the
// Lilka SD card. At boot we list the directory and expose a
// ``GameCart`` array with only the ``name``/``name_len`` fields
// filled, so the existing in-firmware menu can render the picker.
// The actual cart contents are loaded on demand when the user picks
// one (``sd_load_selected_cart``), to keep RAM/PSRAM footprint low.

#include "data.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Directory on the SD card to scan for ``.p8`` carts.
#ifndef PICO8_SD_DIR
#define PICO8_SD_DIR "/Pico8"
#endif

// Max number of carts shown in the picker.
#ifndef PICO8_SD_MAX_CARTS
#define PICO8_SD_MAX_CARTS 32
#endif

// Scan PICO8_SD_DIR on the SD card for ``*.p8`` files and populate the
// internal cart list. Returns the number of carts found (0 if the
// directory is missing/empty or the SD card is not present).
uint8_t   sd_scan_carts(void);

// Pointer to the array of menu-display ``GameCart`` entries. Only
// ``name`` / ``name_len`` are populated until ``sd_load_selected_cart``
// is called for a specific index. Returns NULL before ``sd_scan_carts``.
GameCart* sd_get_cart_array(void);
uint8_t   sd_get_cart_count(void);

// Load the cart at ``index`` (as returned by the scanner) from SD into
// the global "selected cart" buffer. After this returns true, the
// matching entry in ``sd_get_cart_array()[index]`` is updated in-place
// so its ``code``, ``gfx``, ``map``, ``sfx``, ``gff``, ``label``
// pointers are valid for use with ``cartParser`` and ``init_lua``.
bool      sd_load_selected_cart(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
