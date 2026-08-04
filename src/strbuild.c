//
// Created by Tepachitos on 3/22/26.
//
#include <TPF1/TPF_strbuild.h>

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_stdinc.h>

struct TPF_SB {
  char* data;
  size_t len;
  size_t cap;
};

TPF_SB* TPF_CreateSB(void) {
  TPF_SB* sb = SDL_malloc(sizeof(TPF_SB));
  if (sb == NULL) {
    return NULL;
  }

  sb->data = NULL;
  sb->len = 0;
  sb->cap = 0;
  return sb;
}

void TPF_DestroySB(TPF_SB* sb) {
  if (sb != NULL) {
    SDL_free(sb->data);
    SDL_free(sb);
  }
}

bool TPF_SBReserve(TPF_SB* sb, const size_t req) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  if (req <= sb->cap) {
    return true;
  }

  const size_t new_cap = sb->cap + req;
  char* new_data = SDL_realloc(sb->data, new_cap);
  if (new_data == NULL) {
    return false;
  }

  sb->data = new_data;
  sb->cap = new_cap;
  return true;
}

size_t TPF_SBTotalLen(const TPF_SB* sb) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return 0;
  }

  return sb->len;
}

bool TPF_SBAppend(TPF_SB* sb, const TPF_SV sv) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  SDL_assert(TPF_SBReserve(sb, sv.len));
  if (!TPF_SBReserve(sb, sv.len)) {
    return false;
  }

  memcpy(sb->data + sb->len, sv.data, sv.len);
  sb->len += sv.len;
  return true;
}

bool TPF_SBAppendf(TPF_SB* sb, const char* fmt, ...) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  va_list args;
  va_start(args, fmt);
  char* temp = NULL;
  int len = SDL_vasprintf(&temp, fmt, args);
  va_end(args);

  if (len < 0 || temp == NULL) {
    return false;
  }

  bool ok = TPF_SBAppend(sb, TPF_SVFromParts(temp, (size_t)len));
  SDL_free(temp);
  return ok;
}

bool TPF_SBAppendRepeated(TPF_SB* sb, const TPF_SV sv, const Uint32 times) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  SDL_assert(TPF_SBReserve(sb, sv.len * times));
  if (!TPF_SBReserve(sb, sv.len * times)) {
    return false;
  }

  for (Uint32 i = 0; i < times; i++) {
    memcpy(sb->data + sb->len, sv.data, sv.len);
    sb->len += sv.len;
  }
  return true;
}

bool TPF_SBAppendLowered(TPF_SB* sb, const TPF_SV sv) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  SDL_assert(TPF_SBReserve(sb, sv.len));
  if (!TPF_SBReserve(sb, sv.len)) {
    return false;
  }

  for (size_t i = 0; i < sv.len; i++) {
    sb->data[sb->len + i] = (char)SDL_tolower((unsigned char)sv.data[i]);
  }
  sb->len += sv.len;
  return true;
}

bool TPF_SBAppendUppered(TPF_SB* sb, const TPF_SV sv) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  SDL_assert(TPF_SBReserve(sb, sv.len));
  if (!TPF_SBReserve(sb, sv.len)) {
    return false;
  }

  for (size_t i = 0; i < sv.len; i++) {
    sb->data[sb->len + i] = (char)SDL_toupper((unsigned char)sv.data[i]);
  }
  sb->len += sv.len;
  return true;
}

bool TPF_SBAppendReplaced(TPF_SB* sb,
                          const TPF_SV ori_sv,
                          const TPF_SV old_sv,
                          const TPF_SV new_sv,
                          const size_t n) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  if (ori_sv.len == 0) {
    return true;
  }

  // nothing changed
  if (old_sv.len == 0) {
    return TPF_SBAppend(sb, ori_sv);
  }

  size_t old_occurences = TPF_SVCountSV(ori_sv, old_sv);
  size_t to_replace = SDL_min(old_occurences, n);
  size_t total_old = to_replace * old_sv.len;
  size_t total_new = to_replace * new_sv.len;
  size_t needed = ori_sv.len - total_old + total_new;
  if (!TPF_SBReserve(sb, needed)) {
    return false;
  }

  TPF_SV remaining = ori_sv;
  for (size_t repl = 0; repl < to_replace; repl++) {
    size_t upto = 0;
    if (!TPF_SVIndexOfSV(remaining, old_sv, &upto)) {
      break;
    }

    TPF_SV left = {0};
    if (!TPF_SVSub(remaining, 0, upto, &left)) {
      return false;
    }

    if (!TPF_SBAppend(sb, left)) {
      return false;
    }
    if (!TPF_SBAppend(sb, new_sv)) {
      return false;
    }

    size_t after = upto + old_sv.len;
    if (!TPF_SVSub(remaining, after, remaining.len - after, &remaining)) {
      return false;
    }
  }

  return TPF_SBAppend(sb, remaining);
}

bool TPF_SBAppendReplacedAll(TPF_SB* sb,
                             const TPF_SV ori_sv,
                             const TPF_SV old_sv,
                             const TPF_SV new_sv) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  if (ori_sv.len == 0) {
    return true;
  }

  return TPF_SBAppendReplaced(sb, ori_sv, old_sv, new_sv, (size_t)-1);
}

bool TPF_SBAppendJoined(TPF_SB* sb,
                        const size_t sv_count,
                        const TPF_SV* sv,
                        const TPF_SV sep) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return false;
  }

  if (sv_count == 0) {
    return true;
  }

  size_t need = 0;
  for (size_t i = 0; i < sv_count; i++) {
    need += sv[i].len;
    if (i + 1 < sv_count) {
      need += sep.len;
    }
  }

  if (!TPF_SBReserve(sb, need)) {
    return false;
  }

  for (size_t i = 0; i < sv_count; i++) {
    if (!TPF_SBAppend(sb, sv[i])) {
      return false;
    }
    if (i < sv_count - 1) {
      if (!TPF_SBAppend(sb, sep)) {
        return false;
      }
    }
  }

  return true;
}

TPF_SV TPF_SBBuildSV(const TPF_SB* sb) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return TPF_SV_NULL;
  }

  return (TPF_SV){
      .data = sb->data,
      .len = sb->len,
  };
}

char* TPF_SBBuildCStr(const TPF_SB* sb) {
  SDL_assert(sb != NULL);
  if (sb == NULL) {
    return NULL;
  }

  char* data = SDL_malloc(sb->len + 1);
  if (data == NULL) {
    return NULL;
  }

  SDL_memset(data, 0, sb->len + 1);
  SDL_memcpy(data, sb->data, sb->len);
  return data;
}
