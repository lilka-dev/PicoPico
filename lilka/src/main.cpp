/**
 * PicoPico PICO-8 Emulator - Lilka Entry Point
 * 
 * This is the Arduino-style entry point for the Lilka platform.
 * Lilka is a DIY handheld game console based on ESP32-S3.
 * 
 * Hardware:
 * - ESP32-S3-WROOM-1-N16R8
 * - ST7789 Display (280x240)
 * - I2S Audio
 * - 10 buttons (D-pad, A, B, C, D, Start, Select)
 * 
 * Build with PlatformIO:
 *   cd lilka && pio run
 * 
 * Upload:
 *   cd lilka && pio run --target upload
 */

// First, include Arduino headers but save and undefine conflicting macros
#include <Arduino.h>

// Save the original delay function
static inline void arduino_delay(uint32_t ms) { delay(ms); }

// Undefine conflicting macros from Arduino/ctype before including PicoPico code
// These are defined in WCharacter.h and conflict with Lua variable names
#ifdef _L
#undef _L
#endif
#ifdef _U  
#undef _U
#endif
#ifdef _N
#undef _N
#endif
#ifdef _S
#undef _S
#endif
#ifdef _P
#undef _P
#endif
#ifdef _C
#undef _C
#endif
#ifdef _X
#undef _X
#endif
#ifdef _B
#undef _B
#endif

// Undefine binary.h macros that conflict with fix32.h constants
#ifdef B1
#undef B1
#endif
#ifdef A1
#undef A1
#endif
#ifdef C1
#undef C1
#endif

// Define these macros BEFORE including Lua sources so luaconf.h will
// conditionally define luai_writestring, luai_nummod, etc.
// These must be defined without values to match how Lua source files define them
#define LUA_LIB
#define LUA_CORE
#define lobject_c
#define lvm_c

// Include stdio for Lua's print functions
#include <cstdio>
#include <math.h>

#include <lilka.h>

// Include the Lua library sources from esp/lua (better suited for ESP32)
// These must be included BEFORE the main PicoPico code as it depends on Lua
// Note: lmathlib.cpp is not used - math functions are in lpico8lib.cpp
#include "../../esp/lua/lapi.cpp"
#include "../../esp/lua/lauxlib.cpp"
#include "../../esp/lua/lbaselib.cpp"
#include "../../esp/lua/lbitlib.cpp"
#include "../../esp/lua/lcode.cpp"
#include "../../esp/lua/lcorolib.cpp"
#include "../../esp/lua/lctype.cpp"
#include "../../esp/lua/ldblib.cpp"
#include "../../esp/lua/ldebug.cpp"
#include "../../esp/lua/ldo.cpp"
#include "../../esp/lua/ldump.cpp"
#include "../../esp/lua/lfunc.cpp"
#include "../../esp/lua/lgc.cpp"
#include "../../esp/lua/linit.cpp"
#include "../../esp/lua/liolib.cpp"
#include "../../esp/lua/llex.cpp"
#include "../../esp/lua/lmem.cpp"
#include "../../esp/lua/loadlib.cpp"
#include "../../esp/lua/lobject.cpp"
#include "../../esp/lua/lopcodes.cpp"
#include "../../esp/lua/loslib.cpp"
#include "../../esp/lua/lparser.cpp"
#include "../../esp/lua/lpico8lib.cpp"
#include "../../esp/lua/lstate.cpp"
#include "../../esp/lua/lstring.cpp"
#include "../../esp/lua/lstrlib.cpp"
#include "../../esp/lua/ltable.cpp"
#include "../../esp/lua/ltablib.cpp"
#include "../../esp/lua/ltests.cpp"
#include "../../esp/lua/ltm.cpp"
#include "../../esp/lua/lundump.cpp"
#include "../../esp/lua/lvm.cpp"
#include "../../esp/lua/lzio.cpp"

// Include the main PicoPico code
// These are relative to the project root (lilka folder)
#include "../../src/main.cpp"

void setup() {
    // Initialize Lilka hardware (display, buttons, audio, SD card, etc.)
    lilka::begin();
    
    // Print startup message
    Serial.println("PicoPico PICO-8 Emulator for Lilka");
    Serial.println("Starting...");
    
    // Show a brief splash screen
    lilka::display.fillScreen(lilka::colors::Black);
    lilka::display.setTextColor(lilka::colors::White);
    lilka::display.setFont(FONT_10x20);
    lilka::display.setCursor(60, 100);
    lilka::display.println("PicoPico");
    lilka::display.setCursor(40, 130);
    lilka::display.setFont(FONT_6x13);
    lilka::display.println("PICO-8 Emulator");
    
    arduino_delay(1000);  // Use saved Arduino delay
    
    // Clear screen before starting
    lilka::display.fillScreen(lilka::colors::Black);
    
    // Run the PICO-8 emulator main function
    int result = pico8();
    
    if (result != 0) {
        // Error occurred, display message
        lilka::display.fillScreen(lilka::colors::Dark_red);
        lilka::display.setTextColor(lilka::colors::White);
        lilka::display.setCursor(20, 120);
        lilka::display.println("Error starting emulator");
    }
}

void loop() {
    // The main game loop runs in pico8() called from setup()
    // If we get here, restart
    arduino_delay(100);  // Use saved Arduino delay
    
    lilka::State state = lilka::controller.getState();
    if (state.any.justPressed) {
        ESP.restart();
    }
}
