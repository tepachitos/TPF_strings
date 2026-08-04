//
// Created by carlos on <today>.
// Updated for TPF_UTF8Encode/TPF_UTF8Decode API.
//
// clang-format off
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
// clang-format on

#include <SDL3/SDL_stdinc.h>
#include <TPF1/TPF_utf8.h>

#define UNICODE_REPLACEMENT_CHARACTER 0xFFFDu

static void assert_encode(Uint32 cp,
                          const Uint8* expected,
                          size_t expected_len) {
  Uint8 out[8] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

  size_t n = TPF_UTF8Encode(cp, out, sizeof(out));
  assert_int_equal(n, expected_len);

  for (size_t i = 0; i < expected_len; i++) {
    assert_int_equal(out[i], expected[i]);
  }
}

static void assert_decode(const Uint8* bytes,
                          size_t nbytes,
                          size_t expected_len,
                          Uint32 expected_cp) {
  Uint32 cp = 0;
  size_t n = TPF_UTF8Decode(bytes, nbytes, &cp);
  assert_int_equal(n, expected_len);
  assert_int_equal(cp, expected_cp);
}

static void test_encode_basic(void** state) {
  (void)state;

  // 'E' U+0045
  const Uint8 E[] = {0x45};
  assert_encode(0x0045u, E, 1);

  // 'ǂ' U+01C2 -> C7 82
  const Uint8 u01c2[] = {0xC7, 0x82};
  assert_encode(0x01C2u, u01c2, 2);

  // '㊗' U+3297 -> E3 8A 97
  const Uint8 u3297[] = {0xE3, 0x8A, 0x97};
  assert_encode(0x3297u, u3297, 3);

  // '😌' U+1F60C -> F0 9F 98 8C
  const Uint8 u1f60c[] = {0xF0, 0x9F, 0x98, 0x8C};
  assert_encode(0x1F60Cu, u1f60c, 4);
}

static void test_decode_basic(void** state) {
  (void)state;

  const Uint8 E[] = {0x45};
  assert_decode(E, 1, 1, 0x0045u);

  const Uint8 u01c2[] = {0xC7, 0x82};
  assert_decode(u01c2, 2, 2, 0x01C2u);

  const Uint8 u3297[] = {0xE3, 0x8A, 0x97};
  assert_decode(u3297, 3, 3, 0x3297u);

  const Uint8 u1f60c[] = {0xF0, 0x9F, 0x98, 0x8C};
  assert_decode(u1f60c, 4, 4, 0x1F60Cu);
}

static void test_decode_boundaries_valid(void** state) {
  (void)state;

  // U+0000
  const Uint8 u0000[] = {0x00};
  assert_decode(u0000, 1, 1, 0x0000u);

  // U+007F
  const Uint8 u007f[] = {0x7F};
  assert_decode(u007f, 1, 1, 0x007Fu);

  // U+0080 -> C2 80
  const Uint8 u0080[] = {0xC2, 0x80};
  assert_decode(u0080, 2, 2, 0x0080u);

  // U+07FF -> DF BF
  const Uint8 u07ff[] = {0xDF, 0xBF};
  assert_decode(u07ff, 2, 2, 0x07FFu);

  // U+0800 -> E0 A0 80
  const Uint8 u0800[] = {0xE0, 0xA0, 0x80};
  assert_decode(u0800, 3, 3, 0x0800u);

  // U+FFFF -> EF BF BF
  const Uint8 uffff[] = {0xEF, 0xBF, 0xBF};
  assert_decode(uffff, 3, 3, 0xFFFFu);

  // U+10000 -> F0 90 80 80
  const Uint8 u10000[] = {0xF0, 0x90, 0x80, 0x80};
  assert_decode(u10000, 4, 4, 0x10000u);

  // U+10FFFF -> F4 8F BF BF
  const Uint8 u10ffff[] = {0xF4, 0x8F, 0xBF, 0xBF};
  assert_decode(u10ffff, 4, 4, 0x10FFFFu);
}

static void test_encode_rejects_invalid_codepoints(void** state) {
  (void)state;

  Uint8 out[4] = {0, 0, 0, 0};

  // > U+10FFFF
  assert_int_equal(TPF_UTF8Encode(0x110000u, out, sizeof(out)), 0);

  // surrogate range
  assert_int_equal(TPF_UTF8Encode(0xD800u, out, sizeof(out)), 0);
  assert_int_equal(TPF_UTF8Encode(0xDFFFu, out, sizeof(out)), 0);
}

static void test_encode_buffer_too_small(void** state) {
  (void)state;

  Uint8 out[4] = {0xAA, 0xAA, 0xAA, 0xAA};

  // Needs 2 bytes, give only 1
  assert_int_equal(TPF_UTF8Encode(0x0080u, out, 1), 0);

  // Needs 3 bytes, give only 2
  assert_int_equal(TPF_UTF8Encode(0x0800u, out, 2), 0);

  // Needs 4 bytes, give only 3
  assert_int_equal(TPF_UTF8Encode(0x10000u, out, 3), 0);
}

static void test_decode_incomplete_sequences_return_0(void** state) {
  (void)state;

  Uint32 cp = 0xDEADBEEFu;

  // lead indicates 2-byte but only 1 available
  const Uint8 two_lead[] = {0xC2};
  assert_int_equal(TPF_UTF8Decode(two_lead, 1, &cp), 0);

  // 3-byte lead but only 2 available
  const Uint8 three_lead[] = {0xE0, 0xA0};
  assert_int_equal(TPF_UTF8Decode(three_lead, 2, &cp), 0);

  // 4-byte lead but only 3 available
  const Uint8 four_lead[] = {0xF0, 0x90, 0x80};
  assert_int_equal(TPF_UTF8Decode(four_lead, 3, &cp), 0);

  // n==0 -> need more data
  const Uint8 any[] = {0x41};
  assert_int_equal(TPF_UTF8Decode(any, 0, &cp), 0);
}

static void test_decode_malformed_sequences(void** state) {
  (void)state;

  // Unexpected continuation byte as a start
  const Uint8 bad1[] = {0x80};
  assert_decode(bad1, 1, 1, UNICODE_REPLACEMENT_CHARACTER);

  // 2-byte sequence with non-continuation
  const Uint8 bad2[] = {0xC2, 0x20};
  assert_decode(bad2, 2, 1, UNICODE_REPLACEMENT_CHARACTER);

  // 3-byte sequence with non-continuation
  const Uint8 bad3[] = {0xE1, 0x80, 0x41};
  assert_decode(bad3, 3, 1, UNICODE_REPLACEMENT_CHARACTER);

  // 4-byte sequence with non-continuation
  const Uint8 bad4[] = {0xF0, 0x90, 0x41, 0x80};
  assert_decode(bad4, 4, 1, UNICODE_REPLACEMENT_CHARACTER);

  // Invalid lead byte > F4
  const Uint8 bad5[] = {0xF5, 0x80, 0x80, 0x80};
  assert_decode(bad5, 4, 1, UNICODE_REPLACEMENT_CHARACTER);
}

static void test_decode_overlong_sequences(void** state) {
  (void)state;

  // Overlong NUL: C0 80 (should be rejected)
  const Uint8 over1[] = {0xC0, 0x80};
  assert_decode(over1, 2, 1, UNICODE_REPLACEMENT_CHARACTER);

  // Overlong '/' (U+002F): C0 AF
  const Uint8 over2[] = {0xC0, 0xAF};
  assert_decode(over2, 2, 1, UNICODE_REPLACEMENT_CHARACTER);

  // Overlong 3-byte for U+0080: E0 80 80
  const Uint8 over3[] = {0xE0, 0x80, 0x80};
  assert_decode(over3, 3, 1, UNICODE_REPLACEMENT_CHARACTER);

  // Overlong 4-byte for U+0000: F0 80 80 80
  const Uint8 over4[] = {0xF0, 0x80, 0x80, 0x80};
  assert_decode(over4, 4, 1, UNICODE_REPLACEMENT_CHARACTER);
}

static void test_decode_illegal_code_positions(void** state) {
  (void)state;

  // UTF-16 surrogate U+D800: ED A0 80 (must be rejected)
  const Uint8 surr1[] = {0xED, 0xA0, 0x80};
  assert_decode(surr1, 3, 1, UNICODE_REPLACEMENT_CHARACTER);

  // UTF-16 surrogate U+DFFF: ED BF BF (must be rejected)
  const Uint8 surr2[] = {0xED, 0xBF, 0xBF};
  assert_decode(surr2, 3, 1, UNICODE_REPLACEMENT_CHARACTER);

  // > U+10FFFF: F4 90 80 80 (must be rejected)
  const Uint8 gtmax[] = {0xF4, 0x90, 0x80, 0x80};
  assert_decode(gtmax, 4, 1, UNICODE_REPLACEMENT_CHARACTER);
}

int main(void) {
  (void)(jmp_buf*)0;
  (void)(va_list*)0;

  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_encode_basic),
      cmocka_unit_test(test_decode_basic),
      cmocka_unit_test(test_decode_boundaries_valid),
      cmocka_unit_test(test_encode_rejects_invalid_codepoints),
      cmocka_unit_test(test_encode_buffer_too_small),
      cmocka_unit_test(test_decode_incomplete_sequences_return_0),
      cmocka_unit_test(test_decode_malformed_sequences),
      cmocka_unit_test(test_decode_overlong_sequences),
      cmocka_unit_test(test_decode_illegal_code_positions),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
