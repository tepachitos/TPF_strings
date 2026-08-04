#ifndef TPF_UTF8_H
#define TPF_UTF8_H

#include <SDL3/SDL_stdinc.h>

/**
 * @brief Encodes an Unicode Codepoint (ucp) into a UTF8 rune (max 4
 * bytes).
 *
 * @param ucp unicode codepoint to encode.
 * @param out 4-byte (at least) string to write encoding output.
 * @param out_len length remaining on the out buffer (not the total length).
 * @return the length of the final encoded character.
 */
size_t TPF_UTF8Encode(Uint32 ucp, Uint8* out, size_t out_len);

/**
 * @brief Decodes an UTF-8 rune into an Unicode Codepoint (ucp).
 *
 * @param str source string
 * @param n length of the string
 * @param ucp output codepoint.
 * @return the decoded length of the UTF8 rune (how many bytes were read from
 * str). A value of 0 means that there are not enough bytes to decode the
 * codepoint.
 */
size_t TPF_UTF8Decode(const Uint8* str, size_t n, Uint32* ucp);

#endif /* TPF_UTF8_H */
