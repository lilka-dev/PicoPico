#ifndef P8_TEXT_PARSER_C
#define P8_TEXT_PARSER_C

#include "p8_text_parser.h"

#include <stdlib.h>
#include <string.h>

// Optional: prefer PSRAM on ESP32-S3 (Lilka) so we don't blow DRAM.
#if defined(LILKA_BACKEND) || defined(ESP_BACKEND)
  #include <esp_heap_caps.h>
  static inline void* p8_alloc(size_t n) {
      void* p = heap_caps_malloc(n, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
      if (!p) p = malloc(n);
      return p;
  }
  static inline void p8_free(void* p) {
      if (!p) return;
      // heap_caps_free handles both SPIRAM and regular heap
      heap_caps_free(p);
  }
#else
  static inline void* p8_alloc(size_t n) { return malloc(n); }
  static inline void  p8_free(void* p)   { free(p); }
#endif

// Map a single hex character to its nibble value, or 0xFF on error.
static inline uint8_t p8_hex_nibble(char c) {
    if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
    if (c >= 'a' && c <= 'f') return (uint8_t)(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return (uint8_t)(c - 'A' + 10);
    return 0xFF;
}

// Convert a stream of hex digit chars to one nibble per byte (gfx/label).
// Non-hex chars are skipped. Returns the number of nibbles written.
static size_t p8_nibbles(const char* src, size_t src_len, uint8_t* dst) {
    size_t n = 0;
    for (size_t i = 0; i < src_len; ++i) {
        uint8_t v = p8_hex_nibble(src[i]);
        if (v == 0xFF) continue;
        dst[n++] = v;
    }
    return n;
}

// Convert a stream of hex digit chars to one byte per pair (gff/map).
// Non-hex chars are skipped. msn first, then lsn. Returns the number of
// bytes written.
static size_t p8_hex_bytes(const char* src, size_t src_len, uint8_t* dst) {
    size_t n = 0;
    uint8_t high = 0xFF;
    for (size_t i = 0; i < src_len; ++i) {
        uint8_t v = p8_hex_nibble(src[i]);
        if (v == 0xFF) continue;
        if (high == 0xFF) {
            high = v;
        } else {
            dst[n++] = (uint8_t)((high << 4) | v);
            high = 0xFF;
        }
    }
    return n;
}

// Identify which section a header line starts (returns 0 if not a header).
enum P8Section {
    P8_NONE = 0,
    P8_LUA,
    P8_GFX,
    P8_GFF,
    P8_LABEL,
    P8_MAP,
    P8_SFX,
    P8_MUSIC,
};

static int p8_match_header(const char* line, size_t len) {
    // Trim trailing CR
    while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == ' ' || line[len - 1] == '\t')) {
        --len;
    }
    if (len == 7  && memcmp(line, "__lua__",   7)  == 0) return P8_LUA;
    if (len == 7  && memcmp(line, "__gfx__",   7)  == 0) return P8_GFX;
    if (len == 7  && memcmp(line, "__gff__",   7)  == 0) return P8_GFF;
    if (len == 7  && memcmp(line, "__map__",   7)  == 0) return P8_MAP;
    if (len == 7  && memcmp(line, "__sfx__",   7)  == 0) return P8_SFX;
    if (len == 9  && memcmp(line, "__label__", 9)  == 0) return P8_LABEL;
    if (len == 9  && memcmp(line, "__music__", 9)  == 0) return P8_MUSIC;
    return P8_NONE;
}

void p8_cart_free(LoadedCart* cart) {
    if (!cart) return;
    p8_free(cart->code_buf);  cart->code_buf  = NULL;
    p8_free(cart->gfx_buf);   cart->gfx_buf   = NULL;
    p8_free(cart->gff_buf);   cart->gff_buf   = NULL;
    p8_free(cart->map_buf);   cart->map_buf   = NULL;
    p8_free(cart->sfx_buf);   cart->sfx_buf   = NULL;
    p8_free(cart->label_buf); cart->label_buf = NULL;
    memset(&cart->cart, 0, sizeof(cart->cart));
}

bool p8_parse_text(const char* data, size_t len,
                   const char* display_name, LoadedCart* out) {
    if (!data || !out) return false;
    memset(out, 0, sizeof(*out));

    // ---- First pass: find section spans so we can size buffers. ----
    // We don't allocate intermediate copies; just record [start, end)
    // offsets for each section's payload bytes.
    struct Span { size_t start, end; bool present; };
    struct Span spans[8];
    memset(spans, 0, sizeof(spans));

    int cur = P8_NONE;
    size_t cur_payload_start = 0;
    size_t i = 0;
    while (i <= len) {
        // Find next newline or end-of-buffer.
        size_t line_start = i;
        while (i < len && data[i] != '\n') ++i;
        size_t line_end = i; // exclusive, no newline
        // Trim CR
        size_t trimmed_end = line_end;
        if (trimmed_end > line_start && data[trimmed_end - 1] == '\r') --trimmed_end;

        int header = p8_match_header(data + line_start, trimmed_end - line_start);
        if (header) {
            // Close previous section
            if (cur != P8_NONE) {
                spans[cur].end = line_start; // up to (not including) this header line
            }
            cur = header;
            cur_payload_start = (i < len) ? (i + 1) : i;
            spans[cur].start = cur_payload_start;
            spans[cur].end   = cur_payload_start;
            spans[cur].present = true;
        }

        if (i >= len) {
            if (cur != P8_NONE) spans[cur].end = len;
            break;
        }
        ++i; // skip the '\n'
    }

    // ---- LUA: copy raw source (including newlines), NUL-terminate. ----
    if (spans[P8_LUA].present) {
        size_t lua_len = spans[P8_LUA].end - spans[P8_LUA].start;
        // strip trailing whitespace
        while (lua_len > 0) {
            char c = data[spans[P8_LUA].start + lua_len - 1];
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') --lua_len;
            else break;
        }
        out->code_buf = (uint8_t*)p8_alloc(lua_len + 1);
        if (!out->code_buf) goto fail;
        if (lua_len) memcpy(out->code_buf, data + spans[P8_LUA].start, lua_len);
        out->code_buf[lua_len] = 0;
        out->cart.code = out->code_buf;
        out->cart.code_len = (uint16_t)lua_len;
    }

    // ---- GFX: 1 nibble per char (max 128*128 = 16384). ----
    if (spans[P8_GFX].present) {
        size_t gfx_raw = spans[P8_GFX].end - spans[P8_GFX].start;
        if (gfx_raw > 0) {
            out->gfx_buf = (uint8_t*)p8_alloc(gfx_raw); // upper bound
            if (!out->gfx_buf) goto fail;
            size_t n = p8_nibbles(data + spans[P8_GFX].start, gfx_raw, out->gfx_buf);
            out->cart.gfx = out->gfx_buf;
            out->cart.gfx_len = (uint16_t)n;
        }
    }

    // ---- GFF: 2 chars per byte (sprite flags). ----
    if (spans[P8_GFF].present) {
        size_t raw = spans[P8_GFF].end - spans[P8_GFF].start;
        if (raw > 0) {
            out->gff_buf = (uint8_t*)p8_alloc(raw / 2 + 1);
            if (!out->gff_buf) goto fail;
            size_t n = p8_hex_bytes(data + spans[P8_GFF].start, raw, out->gff_buf);
            out->cart.gff = out->gff_buf;
            out->cart.gff_len = (uint16_t)n;
        }
    }

    // ---- LABEL: 1 nibble per char. ----
    if (spans[P8_LABEL].present) {
        size_t raw = spans[P8_LABEL].end - spans[P8_LABEL].start;
        if (raw > 0) {
            out->label_buf = (uint8_t*)p8_alloc(raw);
            if (!out->label_buf) goto fail;
            size_t n = p8_nibbles(data + spans[P8_LABEL].start, raw, out->label_buf);
            out->cart.label = out->label_buf;
            out->cart.label_len = (uint16_t)n;
        }
    }

    // ---- MAP: 2 chars per byte. ----
    if (spans[P8_MAP].present) {
        size_t raw = spans[P8_MAP].end - spans[P8_MAP].start;
        if (raw > 0) {
            out->map_buf = (uint8_t*)p8_alloc(raw / 2 + 1);
            if (!out->map_buf) goto fail;
            size_t n = p8_hex_bytes(data + spans[P8_MAP].start, raw, out->map_buf);
            out->cart.map = out->map_buf;
            out->cart.map_len = (uint16_t)n;
        }
    }

    // ---- SFX: raw concatenated hex text (one entry = 168 chars).
    //      SFXParser() expects the raw text bytes, not parsed nibbles. ----
    if (spans[P8_SFX].present) {
        size_t raw = spans[P8_SFX].end - spans[P8_SFX].start;
        if (raw > 0) {
            out->sfx_buf = (uint8_t*)p8_alloc(raw);
            if (!out->sfx_buf) goto fail;
            // Filter out newlines (keep only hex chars so each SFX is exactly 168 chars).
            size_t n = 0;
            for (size_t k = 0; k < raw; ++k) {
                char c = data[spans[P8_SFX].start + k];
                if (c == '\n' || c == '\r') continue;
                out->sfx_buf[n++] = (uint8_t)c;
            }
            out->cart.sfx = out->sfx_buf;
            out->cart.sfx_len = (uint16_t)n;
        }
    }

    // ---- Display name. ----
    {
        const char* n = display_name ? display_name : "cart";
        size_t nl = strlen(n);
        if (nl >= sizeof(out->name_buf)) nl = sizeof(out->name_buf) - 1;
        memcpy(out->name_buf, n, nl);
        out->name_buf[nl] = 0;
        out->cart.name = out->name_buf;
        out->cart.name_len = (uint8_t)nl;
    }

    return out->code_buf != NULL; // a cart without code is useless

fail:
    p8_cart_free(out);
    return false;
}

#endif // P8_TEXT_PARSER_C
