//
// Created by Tepachitos on 3/22/26.
//
// clang-format off
#include <cmocka.h>
// clang-format on

#include <TPF1/TPF_strview.h>

// Helpers
static TPF_SV SVLIT(const char* s) {
  return TPF_SVFromCStr(s);
}

static void test_from_parts_and_from_cstr(void** state) {
  (void)state;

  TPF_SV a = TPF_SVFromParts("abc", 3);
  assert_non_null(a.data);
  assert_int_equal(a.len, 3);

  TPF_SV n = TPF_SVFromParts(NULL, 123);
  assert_null(n.data);
  assert_int_equal(n.len, 0);

  TPF_SV b = TPF_SVFromCStr("hello");
  assert_non_null(b.data);
  assert_int_equal(b.len, 5);

  TPF_SV z = TPF_SVFromCStr(NULL);
  assert_null(z.data);
  assert_int_equal(z.len, 0);
}

static void test_sub_and_validity(void** state) {
  (void)state;

  TPF_SV a = SVLIT("abcdef");

  TPF_SV sub = {0};
  assert_true(TPF_SVSub(a, 2, 3, &sub));
  assert_int_equal(sub.len, 3);
  assert_memory_equal(sub.data, "cde", 3);

  // out of range
  assert_false(TPF_SVSub(a, 10, 1, &sub));
  assert_false(TPF_SVSub(a, 2, 99, &sub));
  assert_false(TPF_SVSub(TPF_SV_NULL, 0, 0, &sub));

  assert_false(TPF_SVIsNULL(a));
  assert_false(TPF_SVIsEmpty(a));
  assert_true(TPF_SVIsNULL(TPF_SV_NULL));
  assert_true(TPF_SVIsEmpty(TPF_SV_EMPTY));
}

static void test_valid_utf(void** state) {
  (void)state;

  // ASCII valid
  assert_true(TPF_SVIsValidUTF(SVLIT("hello")));

  // Empty / NULL per docs: valid
  assert_true(TPF_SVIsValidUTF(TPF_SV_EMPTY));
  assert_true(TPF_SVIsValidUTF(TPF_SV_NULL));

  // Invalid UTF-8: lone continuation byte 0x80
  const unsigned char bad_bytes[] = {0x80, 0x00};
  TPF_SV bad = TPF_SVFromParts((const char*)bad_bytes, 1);
  assert_false(TPF_SVIsValidUTF(bad));
}

static void test_index_of_byte_and_sv(void** state) {
  (void)state;

  TPF_SV a = SVLIT("abca");

  size_t idx = 999;
  assert_true(TPF_SVIndexOf(a, (unsigned char)'c', &idx));
  assert_int_equal(idx, 2);

  idx = 999;
  assert_false(TPF_SVIndexOf(a, (unsigned char)'z', &idx));

  // substring
  TPF_SV needle = SVLIT("bc");
  idx = 999;
  assert_true(TPF_SVIndexOfSV(a, needle, &idx));
  assert_int_equal(idx, 1);

  // empty needle => index 0
  idx = 999;
  assert_true(TPF_SVIndexOfSV(a, TPF_SV_EMPTY, &idx));
  assert_int_equal(idx, 0);

  // NULL needle => false
  assert_false(TPF_SVIndexOfSV(a, TPF_SV_NULL, &idx));
  assert_false(TPF_SVIndexOfSV(TPF_SV_NULL, needle, &idx));
}

static void test_last_index_of_byte_and_sv(void** state) {
  (void)state;

  TPF_SV a = SVLIT("abca");

  size_t idx = 999;
  assert_true(TPF_SVLastIndexOf(a, (unsigned char)'a', &idx));
  assert_int_equal(idx, 3);

  assert_false(TPF_SVLastIndexOf(a, (unsigned char)'z', &idx));

  // substring last occurrence
  TPF_SV hay = SVLIT("ababa");
  TPF_SV ndl = SVLIT("aba");
  idx = 999;
  assert_true(TPF_SVLastIndexOfSV(hay, ndl, &idx));
  assert_int_equal(idx, 2);

  // empty needle => index a.len (per doc note)
  idx = 999;
  assert_true(TPF_SVLastIndexOfSV(hay, TPF_SV_EMPTY, &idx));
  assert_int_equal(idx, hay.len);

  // NULL needle/hay => false
  assert_false(TPF_SVLastIndexOfSV(hay, TPF_SV_NULL, &idx));
  assert_false(TPF_SVLastIndexOfSV(TPF_SV_NULL, ndl, &idx));
}

static void test_equal_prefix_suffix(void** state) {
  (void)state;

  TPF_SV a = SVLIT("hello");
  TPF_SV b = SVLIT("hello");
  TPF_SV c = SVLIT("hell");
  TPF_SV d = SVLIT("ello");

  assert_true(TPF_SVEqual(a, b));
  assert_false(TPF_SVEqual(a, c));
  assert_false(TPF_SVEqual(TPF_SV_NULL, TPF_SV_EMPTY));
  assert_true(TPF_SVEqual(TPF_SV_NULL, TPF_SV_NULL));

  assert_true(TPF_SVHasPrefix(a, c));
  assert_false(TPF_SVHasPrefix(a, d));

  assert_true(TPF_SVHasSuffix(a, d));
  assert_false(TPF_SVHasSuffix(a, c));
}

static void test_count_byte_and_sv(void** state) {
  (void)state;

  TPF_SV a = SVLIT("aaaa");
  assert_int_equal(TPF_SVCount(a, (unsigned char)'a'), 4);
  assert_int_equal(TPF_SVCount(a, (unsigned char)'b'), 0);

  // overlapping count
  TPF_SV ndl = SVLIT("aa");
  assert_int_equal(TPF_SVCountSV(a, ndl), 3);

  // single-byte needle uses fast path
  TPF_SV ndl1 = SVLIT("a");
  assert_int_equal(TPF_SVCountSV(a, ndl1), 4);

  assert_int_equal(TPF_SVCountSV(TPF_SV_NULL, ndl), 0);
  assert_int_equal(TPF_SVCountSV(a, TPF_SV_NULL), 0);
  assert_int_equal(TPF_SVCountSV(a, TPF_SV_EMPTY), 0);
}

static void test_cut_and_cutsv(void** state) {
  (void)state;

  // Cut byte delimiter (exclude delimiter)
  TPF_SV a = SVLIT("a,b");
  TPF_SV before = {0}, after = {0};

  assert_true(TPF_SVCut(a, (unsigned char)',', &before, &after));
  assert_int_equal(before.len, 1);
  assert_memory_equal(before.data, "a", 1);
  assert_int_equal(after.len, 1);
  assert_memory_equal(after.data, "b", 1);

  // delimiter not found
  assert_false(TPF_SVCut(SVLIT("abc"), (unsigned char)',', &before, &after));

  // Cut substring delimiter
  TPF_SV s = SVLIT("foo--bar");
  TPF_SV delim = SVLIT("--");
  assert_true(TPF_SVCutSV(s, delim, &before, &after));
  assert_int_equal(before.len, 3);
  assert_memory_equal(before.data, "foo", 3);
  assert_int_equal(after.len, 3);
  assert_memory_equal(after.data, "bar", 3);

  // empty delimiter rejected (per doc note)
  assert_false(TPF_SVCutSV(s, TPF_SV_EMPTY, &before, &after));
}

static void test_trim(void** state) {
  (void)state;

  // cutset = whitespace bytes (simple)
  TPF_SV cutset = SVLIT(" \t\n\r");

  TPF_SV a = SVLIT(" \t hi \n");
  TPF_SV tp = TPF_SVTrimPrefix(a, cutset);
  assert_memory_equal(tp.data, "hi \n", 4);
  assert_int_equal(tp.len, 4);

  TPF_SV ts = TPF_SVTrimSuffix(a, cutset);
  // original starts with whitespace; suffix trim should keep prefix intact
  assert_int_equal(ts.len, a.len - 2);  // trims " \n" at end (2 chars)
  assert_memory_equal(ts.data, a.data, ts.len);

  TPF_SV t = TPF_SVTrim(a, cutset);
  assert_int_equal(t.len, 2);
  assert_memory_equal(t.data, "hi", 2);
}

static void test_split_next(void** state) {
  (void)state;

  TPF_SV a = SVLIT("a,b,c");
  size_t off = 0;
  TPF_SV tok = {0};

  assert_true(TPF_SVSplitNext(a, (unsigned char)',', &off, &tok));
  assert_int_equal(tok.len, 1);
  assert_memory_equal(tok.data, "a", 1);

  assert_true(TPF_SVSplitNext(a, (unsigned char)',', &off, &tok));
  assert_int_equal(tok.len, 1);
  assert_memory_equal(tok.data, "b", 1);

  assert_true(TPF_SVSplitNext(a, (unsigned char)',', &off, &tok));
  assert_int_equal(tok.len, 1);
  assert_memory_equal(tok.data, "c", 1);

  // done
  assert_false(TPF_SVSplitNext(a, (unsigned char)',', &off, &tok));

  // trailing delimiter: no empty token (per doc note)
  TPF_SV b = SVLIT("x,");
  off = 0;
  assert_true(TPF_SVSplitNext(b, (unsigned char)',', &off, &tok));
  assert_int_equal(tok.len, 1);
  assert_memory_equal(tok.data, "x", 1);
  assert_false(TPF_SVSplitNext(b, (unsigned char)',', &off, &tok));
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_from_parts_and_from_cstr),
      cmocka_unit_test(test_sub_and_validity),
      cmocka_unit_test(test_valid_utf),
      cmocka_unit_test(test_index_of_byte_and_sv),
      cmocka_unit_test(test_last_index_of_byte_and_sv),
      cmocka_unit_test(test_equal_prefix_suffix),
      cmocka_unit_test(test_count_byte_and_sv),
      cmocka_unit_test(test_cut_and_cutsv),
      cmocka_unit_test(test_trim),
      cmocka_unit_test(test_split_next),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
