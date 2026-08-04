//
// Created by Tepachitos on 3/22/26.
//

#include <TPF1/TPF_strview.h>

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_stdinc.h>

static bool is_valid_utf8_bounded(const unsigned char* s, size_t n) {
  size_t i = 0;
  while (i < n) {
    unsigned char c = s[i];

    if (c <= 0x7F) {  // ASCII
      i += 1;
      continue;
    }

    // Determine sequence length
    size_t need = 0;
    Uint32 min = 0;

    if ((c & 0xE0) == 0xC0) {
      need = 2;
      min = 0x80;
    } else if ((c & 0xF0) == 0xE0) {
      need = 3;
      min = 0x800;
    } else if ((c & 0xF8) == 0xF0) {
      need = 4;
      min = 0x10000;
    } else
      return false;

    if (i + need > n)
      return false;

    Uint32 cp = c & ((1u << (8 - need - 1)) - 1);
    for (size_t k = 1; k < need; k++) {
      unsigned char cc = s[i + k];
      if ((cc & 0xC0) != 0x80)
        return false;
      cp = (cp << 6) | (cc & 0x3F);
    }

    // Reject overlong encodings
    if (cp < min) {
      return false;
    }

    // Reject surrogate halves and out-of-range
    if (cp >= 0xD800 && cp <= 0xDFFF) {
      return false;
    }

    if (cp > 0x10FFFF) {
      return false;
    }

    i += need;
  }

  return true;
}

TPF_SV TPF_SVFromParts(const char* data, size_t len) {
  if (data == NULL) {
    len = 0;
  }

  return (TPF_SV){
      .data = data,
      .len = len,
  };
}

TPF_SV TPF_SVFromCStr(const char* cstr) {
  if (cstr == NULL) {
    return TPF_SV_NULL;
  }

  return TPF_SVFromParts(cstr, SDL_strnlen(cstr, TPF_SV_DEFAULT_MAX_STRLEN));
}

bool TPF_SVSub(const TPF_SV a, size_t off, size_t len, TPF_SV* b) {
  if (b == NULL || a.data == NULL || off > a.len || len > a.len - off) {
    return false;
  }

  *b = (TPF_SV){
      .data = a.data + off,
      .len = len,
  };
  return true;
}

bool TPF_SVIsNULL(const TPF_SV a) {
  return a.data == NULL;
}

bool TPF_SVIsEmpty(const TPF_SV a) {
  return a.len == 0;
}

bool TPF_SVIsValidUTF(const TPF_SV a) {
  if (TPF_SVIsNULL(a) || TPF_SVIsEmpty(a)) {
    return true;
  }

  return is_valid_utf8_bounded((const unsigned char*)a.data, a.len);
}

bool TPF_SVIndexOf(const TPF_SV a, unsigned char c, size_t* index) {
  if (TPF_SVIsNULL(a) || TPF_SVIsEmpty(a)) {
    return false;
  }

  size_t i = 0;
  const unsigned char* data = (const unsigned char*)a.data;
  while (i < a.len && data[i] != c) {
    i += 1;
  }

  if (i < a.len) {
    if (index != NULL) {
      *index = i;
    }

    return true;
  }

  return false;
}

bool TPF_SVIndexOfSV(const TPF_SV a, const TPF_SV b, size_t* index) {
  if (a.data == NULL || b.data == NULL) {
    return false;
  }

  if (b.len == 0) {
    if (index != NULL) {
      *index = 0;
    }
    return true;
  }

  if (b.len > a.len) {
    return false;
  }

  if (b.len == 1) {
    return TPF_SVIndexOf(a, (unsigned char)b.data[0], index);
  }

  for (size_t i = 0; i + b.len <= a.len; i++) {
    if (SDL_memcmp(a.data + i, b.data, b.len) == 0) {
      if (index != NULL) {
        *index = i;
      }

      return true;
    }
  }

  return false;
}

bool TPF_SVLastIndexOf(const TPF_SV a, unsigned char c, size_t* index) {
  if (TPF_SVIsNULL(a) || TPF_SVIsEmpty(a)) {
    return false;
  }

  const unsigned char* data = (const unsigned char*)a.data;
  for (size_t i = a.len; i-- > 0;) {  // wont underflow
    if (data[i] == c) {
      if (index)
        *index = i;
      return true;
    }
  }

  return false;
}

bool TPF_SVLastIndexOfSV(const TPF_SV a, const TPF_SV b, size_t* index) {
  if (a.data == NULL || b.data == NULL) {
    return false;
  }

  if (b.len == 0) {
    if (index) {
      *index = a.len;
    }
    return true;
  }

  if (b.len > a.len) {
    return false;
  }

  if (b.len == 1) {
    return TPF_SVLastIndexOf(a, (unsigned char)b.data[0], index);
  }

  for (size_t pos = a.len - b.len + 1; pos-- > 0;) {  // wont underflow
    if (SDL_memcmp(a.data + pos, b.data, b.len) == 0) {
      if (index)
        *index = pos;
      return true;
    }
  }
  return false;
}

bool TPF_SVEqual(const TPF_SV a, const TPF_SV b) {
  if (a.data == NULL || b.data == NULL) {
    return a.data == b.data && a.len == b.len;
  }

  if (a.len != b.len) {
    return false;
  }

  if (a.len == 0) {
    return true;
  }

  return SDL_memcmp(a.data, b.data, a.len) == 0;
}

bool TPF_SVHasPrefix(const TPF_SV a, const TPF_SV b) {
  if (a.len < b.len) {
    return false;
  }

  TPF_SV heading = {0};
  if (!TPF_SVSub(a, 0, b.len, &heading)) {
    return false;
  }

  return TPF_SVEqual(heading, b);
}

bool TPF_SVHasSuffix(const TPF_SV a, const TPF_SV b) {
  if (a.len < b.len) {
    return false;
  }

  TPF_SV trail = {0};
  if (!TPF_SVSub(a, a.len - b.len, b.len, &trail)) {
    return false;
  }

  return TPF_SVEqual(trail, b);
}

size_t TPF_SVCount(const TPF_SV a, unsigned char c) {
  if (TPF_SVIsNULL(a) || TPF_SVIsEmpty(a)) {
    return 0;
  }

  size_t count = 0;
  size_t i = 0;
  const unsigned char* data = (const unsigned char*)a.data;
  while (i < a.len) {
    if (data[i] == c) {
      count += 1;
    }

    i += 1;
  }

  return count;
}

size_t TPF_SVCountSV(const TPF_SV a, const TPF_SV b) {
  if (a.data == NULL || b.data == NULL || a.len == 0 || b.len == 0 ||
      b.len > a.len) {
    return 0;
  }

  if (b.len == 1) {
    return TPF_SVCount(a, (unsigned char)b.data[0]);
  }

  size_t count = 0;
  for (size_t i = 0; i + b.len <= a.len; i++) {
    if (SDL_memcmp(a.data + i, b.data, b.len) == 0) {
      count++;
    }
  }
  return count;
}

bool TPF_SVCut(const TPF_SV a, unsigned char c, TPF_SV* before, TPF_SV* after) {
  if (TPF_SVIsNULL(a) || TPF_SVIsEmpty(a)) {
    return false;
  }

  size_t i = 0;
  const unsigned char* data = (const unsigned char*)a.data;
  while (i < a.len) {
    if (data[i] == c) {
      bool before_ok = true;
      bool after_ok = true;

      if (before != NULL) {
        before_ok = TPF_SVSub(a, 0, i, before);
      }

      if (after != NULL) {
        after_ok = TPF_SVSub(a, i + 1, a.len - (i + 1), after);
      }

      SDL_assert(before_ok && after_ok);
      return before_ok && after_ok;
    }

    i += 1;
  }

  return false;
}

bool TPF_SVCutSV(const TPF_SV a,
                 const TPF_SV b,
                 TPF_SV* before,
                 TPF_SV* after) {
  if (a.data == NULL || b.data == NULL || a.len == 0 || b.len == 0 ||
      b.len > a.len) {
    return false;
  }

  if (b.len == 1) {
    return TPF_SVCut(a, (unsigned char)b.data[0], before, after);
  }

  for (size_t i = 0; i + b.len <= a.len; i++) {
    if (SDL_memcmp(a.data + i, b.data, b.len) == 0) {
      bool before_ok = true;
      bool after_ok = true;

      if (before != NULL) {
        before_ok = TPF_SVSub(a, 0, i, before);
      }

      if (after != NULL) {
        after_ok = TPF_SVSub(a, i + b.len, a.len - (i + b.len), after);
      }

      SDL_assert(before_ok && after_ok);
      return before_ok && after_ok;
    }
  }

  return false;
}

TPF_SV TPF_SVTrim(const TPF_SV a, const TPF_SV cutset) {
  TPF_SV r = {0};
  r = TPF_SVTrimPrefix(a, cutset);
  r = TPF_SVTrimSuffix(r, cutset);
  return r;
}

TPF_SV TPF_SVTrimPrefix(const TPF_SV a, const TPF_SV cutset) {
  if (a.data == NULL) {
    return TPF_SV_NULL;
  }

  if (a.len == 0) {
    return a;
  }

  if (cutset.data == NULL || cutset.len == 0) {
    return a;
  }

  TPF_SV r = a;
  while (r.len > 0) {
    unsigned char ch = (unsigned char)r.data[0];
    if (!TPF_SVIndexOf(cutset, ch, NULL)) {
      break;
    }
    r.data += 1;
    r.len -= 1;
  }
  return r;
}

TPF_SV TPF_SVTrimSuffix(const TPF_SV a, const TPF_SV cutset) {
  if (a.data == NULL) {
    return TPF_SV_NULL;
  }

  if (a.len == 0) {
    return a;
  }

  if (cutset.data == NULL || cutset.len == 0) {
    return a;
  }

  TPF_SV r = a;
  while (r.len > 0) {
    unsigned char ch = (unsigned char)r.data[r.len - 1];
    if (!TPF_SVIndexOf(cutset, ch, NULL)) {
      break;
    }
    r.len -= 1;
  }
  return r;
}

bool TPF_SVSplitNext(const TPF_SV a,
                     unsigned char c,
                     size_t* off,
                     TPF_SV* tok) {
  SDL_assert(tok && off);
  if (tok == NULL || off == NULL) {
    return false;
  }

  if (a.data == NULL) {
    return false;
  }

  if (*off >= a.len) {
    return false;
  }

  TPF_SV rest = {0};
  if (!TPF_SVSub(a, *off, a.len - *off, &rest)) {
    return false;
  }

  TPF_SV after = {0};
  if (TPF_SVCut(rest, c, tok, &after)) {
    // delimiter found: advance by (before + delimiter)
    size_t consumed = rest.len - after.len;  // == result->len + 1
    *off += consumed;
    return true;
  }

  // delimiter not found: last token is the rest
  *tok = rest;
  *off = a.len;
  return true;
}
