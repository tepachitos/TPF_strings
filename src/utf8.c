#include <TPF1/TPF_utf8.h>

#include <SDL3/SDL_assert.h>
#define UNICODE_REPLACEMENT_CHARACTER 0xFFFDu

static inline bool is_cont(Uint8 b) {
  return (b & 0xC0u) == 0x80u;
}

size_t TPF_UTF8Encode(Uint32 ucp, Uint8* out, size_t out_len) {
  if (ucp > 0x10FFFFu) {
    return 0;
  }

  if (ucp >= 0xD800u && ucp <= 0xDFFFu) {
    return 0;
  }

  if (ucp < 0x80u && out_len > 0) {
    out[0] = (Uint8)ucp;
    return 1;
  }

  if (ucp < 0x800u && out_len > 1) {
    out[0] = (Uint8)((ucp >> 6) | 0xC0u);
    out[1] = (Uint8)((ucp & 0x3Fu) | 0x80u);
    return 2;
  }

  if (ucp < 0x10000u && out_len > 2) {
    out[0] = (Uint8)((ucp >> 12) | 0xE0u);
    out[1] = (Uint8)(((ucp >> 6) & 0x3Fu) | 0x80u);
    out[2] = (Uint8)((ucp & 0x3Fu) | 0x80u);
    return 3;
  }

  if (ucp < 0x110000u && out_len > 3) {
    out[0] = (Uint8)((ucp >> 18) | 0xF0u);
    out[1] = (Uint8)(((ucp >> 12) & 0x3Fu) | 0x80u);
    out[2] = (Uint8)(((ucp >> 6) & 0x3Fu) | 0x80u);
    out[3] = (Uint8)((ucp & 0x3Fu) | 0x80u);
    return 4;
  }

  return 0;
}

size_t TPF_UTF8Decode(const Uint8* str, size_t n, Uint32* ucp) {
  SDL_assert_paranoid(str != NULL && ucp != NULL);
  if (str == NULL || ucp == NULL || n == 0) {
    return 0;
  }

  Uint8 b0 = str[0];
  if (b0 <= 0x7Fu) {
    *ucp = b0;
    return 1;
  }

  if (b0 >= 0xC2u && b0 <= 0xDFu) {
    if (n < 2) {
      return 0;
    }

    Uint8 b1 = str[1];
    if (!is_cont(b1)) {
      goto fail;
    }

    *ucp = ((Uint32)(b0 & 0x1Fu) << 6) | (Uint32)(b1 & 0x3Fu);
    return 2;
  }

  if (b0 >= 0xE0u && b0 <= 0xEFu) {
    if (n < 3) {
      return 0;
    }

    Uint8 b1 = str[1], b2 = str[2];
    if (!is_cont(b1) || !is_cont(b2)) {
      goto fail;
    }

    if (b0 == 0xE0u && b1 < 0xA0u) {
      goto fail;  // overlong
    }

    if (b0 == 0xEDu && b1 >= 0xA0u) {
      goto fail;  // surrogates
    }

    *ucp = ((Uint32)(b0 & 0x0Fu) << 12) | ((Uint32)(b1 & 0x3Fu) << 6) |
           ((Uint32)(b2 & 0x3Fu));
    return 3;
  }

  if (b0 >= 0xF0u && b0 <= 0xF4u) {
    if (n < 4) {
      return 0;
    }

    Uint8 b1 = str[1], b2 = str[2], b3 = str[3];
    if (!is_cont(b1) || !is_cont(b2) || !is_cont(b3)) {
      goto fail;
    }

    if (b0 == 0xF0u && b1 < 0x90u) {
      goto fail;  // overlong
    }

    if (b0 == 0xF4u && b1 > 0x8Fu) {
      goto fail;  // > U+10FFFF
    }

    *ucp = ((Uint32)(b0 & 0x07u) << 18) | ((Uint32)(b1 & 0x3Fu) << 12) |
           ((Uint32)(b2 & 0x3Fu) << 6) | ((Uint32)(b3 & 0x3Fu));
    return 4;
  }

fail:
  *ucp = UNICODE_REPLACEMENT_CHARACTER;
  return 1;
}
