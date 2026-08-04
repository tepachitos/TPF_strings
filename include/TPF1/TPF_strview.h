//
// Created by Tepachitos on 3/22/26.
//

#ifndef TPF_STRVIEW_H
#define TPF_STRVIEW_H

#include <SDL3/SDL_stdinc.h>

/**
 * @brief Maximum length used when converting C strings into string views.
 *
 * Uses SIZE_MAX semantics to scan until NUL.
 */
#define TPF_SV_DEFAULT_MAX_STRLEN ((size_t)-1)

/**
 * @brief printf-style format string for string views.
 *
 * Intended to be used together with TPF_SV_ARG.
 */
#define TPF_SV_FMT "%.*s"

/**
 * @brief printf-style argument helper for string views.
 *
 * Safely clamps length to SDL_MAX_SINT32 and maps NULL data to empty string.
 */
#define TPF_SV_ARG(sv)                                                         \
  (Sint32)((sv).len > (size_t)SDL_MAX_SINT32 ? SDL_MAX_SINT32 : (sv).len),     \
      ((sv).data ? (sv).data : "")

/**
 * @brief Empty (non-NULL) string view.
 *
 * Represents a valid empty string.
 */
#define TPF_SV_EMPTY ((TPF_SV){.data = "", .len = 0})

/**
 * @brief NULL / invalid string view.
 *
 * Represents an invalid or uninitialized view.
 */
#define TPF_SV_NULL ((TPF_SV){.data = NULL, .len = 0})

/**
 * @brief Immutable string view.
 *
 * Non-owning reference to a contiguous byte range.
 * Length is explicit and does not require NUL termination.
 */
typedef struct TPF_SV {
  const char *data;
  size_t len;
} TPF_SV;

/**
 * @brief Construct a string view from raw parts.
 *
 * If @p data is NULL, the resulting view will have length 0.
 *
 * @param data Pointer to string data.
 * @param len  Length in bytes.
 * @return Constructed string view.
 */
TPF_SV TPF_SVFromParts(const char *data, size_t len);

/**
 * @brief Construct a string view from a C string.
 *
 * Scans until NUL or TPF_SV_DEFAULT_MAX_STRLEN.
 *
 * @param cstr NUL-terminated string.
 * @return String view, or TPF_SV_NULL if @p cstr is NULL.
 */
TPF_SV TPF_SVFromCStr(const char *cstr);

/**
 * @brief Create a sub-view of a string view.
 *
 * @param a    Source string view.
 * @param off  Byte offset into @p a.
 * @param len  Length of sub-view.
 * @param b    Output sub-view.
 * @return true if the sub-view is valid, false otherwise.
 */
bool TPF_SVSub(TPF_SV a, size_t off, size_t len, TPF_SV *b);

/**
 * @brief Check whether a string view is NULL / invalid.
 *
 * @param a String view.
 * @return true if the view is NULL.
 */
bool TPF_SVIsNULL(TPF_SV a);

/**
 * @brief Check whether a string view is empty.
 *
 * @param a String view.
 * @return true if the view length is zero.
 */
bool TPF_SVIsEmpty(TPF_SV a);

/**
 * @brief Check whether a string view contains valid UTF-8.
 *
 * This function validates UTF-8 byte sequences within the view bounds.
 *
 * @param a String view.
 * @return true if valid UTF-8 or empty/NULL.
 */
bool TPF_SVIsValidUTF(TPF_SV a);

/**
 * @brief Find the first occurrence of a byte.
 *
 * @param a     String view to search.
 * @param c     Byte to search for.
 * @param index Optional output index.
 * @return true if found.
 */
bool TPF_SVIndexOf(TPF_SV a, unsigned char c, size_t *index);

/**
 * @brief Find the first occurrence of a sub-string.
 *
 * @param a     String view to search.
 * @param b     Needle string view.
 * @param index Optional output index.
 * @return true if found.
 *
 * @note Empty needle matches index 0.
 */
bool TPF_SVIndexOfSV(TPF_SV a, TPF_SV b, size_t *index);

/**
 * @brief Find the last occurrence of a byte.
 *
 * @param a     String view to search.
 * @param c     Byte to search for.
 * @param index Optional output index.
 * @return true if found.
 */
bool TPF_SVLastIndexOf(TPF_SV a, unsigned char c, size_t *index);

/**
 * @brief Find the last occurrence of a sub-string.
 *
 * @param a     String view to search.
 * @param b     Needle string view.
 * @param index Optional output index.
 * @return true if found.
 *
 * @note Empty needle currently matches index a.len.
 */
bool TPF_SVLastIndexOfSV(TPF_SV a, TPF_SV b, size_t *index);

/**
 * @brief Compare two string views for equality.
 *
 * @param a First string view.
 * @param b Second string view.
 * @return true if both views have identical contents.
 *
 * @note NULL views are only equal to other NULL views.
 */
bool TPF_SVEqual(TPF_SV a, TPF_SV b);

/**
 * @brief Check whether a string view starts with another.
 *
 * @param a String view.
 * @param b Prefix.
 * @return true if @p a starts with @p b.
 */
bool TPF_SVHasPrefix(TPF_SV a, TPF_SV b);

/**
 * @brief Check whether a string view ends with another.
 *
 * @param a String view.
 * @param b Suffix.
 * @return true if @p a ends with @p b.
 */
bool TPF_SVHasSuffix(TPF_SV a, TPF_SV b);

/**
 * @brief Count occurrences of a byte.
 *
 * @param a String view.
 * @param c Byte to count.
 * @return Number of occurrences.
 */
size_t TPF_SVCount(TPF_SV a, unsigned char c);

/**
 * @brief Count occurrences of a sub-string.
 *
 * Counts overlapping occurrences.
 *
 * @param a String view.
 * @param b Needle string view.
 * @return Number of occurrences.
 *
 * @note Overlapping behavior is intentional.
 */
size_t TPF_SVCountSV(TPF_SV a, TPF_SV b);

/**
 * @brief Split a string view on the first occurrence of a byte delimiter.
 *
 * The delimiter is not included in the result.
 *
 * @param a      Source string view.
 * @param c      Delimiter byte.
 * @param before Output view before the delimiter (optional).
 * @param after  Output view after the delimiter (optional).
 * @return true if delimiter was found.
 */
bool TPF_SVCut(TPF_SV a, unsigned char c, TPF_SV *before, TPF_SV *after);

/**
 * @brief Split a string view on the first occurrence of a sub-string delimiter.
 *
 * The delimiter is not included in the result.
 *
 * @param a      Source string view.
 * @param b      Delimiter string view.
 * @param before Output view before the delimiter (optional).
 * @param after  Output view after the delimiter (optional).
 * @return true if delimiter was found.
 *
 * @note Empty delimiter is currently rejected.
 */
bool TPF_SVCutSV(TPF_SV a, TPF_SV b, TPF_SV *before, TPF_SV *after);

/**
 * @brief Trim a string view on both sides using a cutset.
 *
 * Removes all leading and trailing bytes present in @p cutset.
 *
 * @param a       Source string view.
 * @param cutset  Set of bytes to trim.
 * @return Trimmed string view.
 */
TPF_SV TPF_SVTrim(TPF_SV a, TPF_SV cutset);

/**
 * @brief Trim a string view prefix using a cutset.
 *
 * Removes all leading bytes present in @p cutset.
 *
 * @param a       Source string view.
 * @param cutset  Set of bytes to trim.
 * @return Trimmed string view.
 */
TPF_SV TPF_SVTrimPrefix(TPF_SV a, TPF_SV cutset);

/**
 * @brief Trim a string view suffix using a cutset.
 *
 * Removes all trailing bytes present in @p cutset.
 *
 * @param a       Source string view.
 * @param cutset  Set of bytes to trim.
 * @return Trimmed string view.
 */
TPF_SV TPF_SVTrimSuffix(TPF_SV a, TPF_SV cutset);

/**
 * @brief Iterate over delimited tokens in a string view.
 *
 * Call repeatedly with the same @p off to retrieve successive tokens.
 *
 * @param a      Source string view.
 * @param c      Delimiter byte.
 * @param off    In/out byte offset.
 * @param result Output token.
 * @return true if a token was produced.
 *
 * @note Trailing delimiters do not currently produce empty tokens.
 */
bool TPF_SVSplitNext(TPF_SV a, unsigned char c, size_t *off, TPF_SV *result);

#endif // TPF_STRVIEW_H
