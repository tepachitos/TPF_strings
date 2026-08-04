//
// Created by Tepachitos on 3/27/26.
//

// clang-format off
#include <cmocka.h>
// clang-format on

#include <SDL3/SDL_stdinc.h>
#include <TPF1/TPF_strbuild.h>

// Helpers
static TPF_SV SVLIT(const char* s) {
  return TPF_SVFromCStr(s);
}

static void test_basic_lifecycle(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();
  assert_non_null(sb);
  assert_int_equal(TPF_SBTotalLen(sb), 0);
  TPF_DestroySB(sb);
}

static void test_append_and_build(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  assert_true(TPF_SBAppend(sb, SVLIT("Hello")));
  assert_int_equal(TPF_SBTotalLen(sb), 5);

  assert_true(TPF_SBAppend(sb, SVLIT(" ")));
  assert_true(TPF_SBAppend(sb, SVLIT("World")));
  assert_int_equal(TPF_SBTotalLen(sb), 11);

  TPF_SV sv = TPF_SBBuildSV(sb);
  assert_int_equal(sv.len, 11);
  assert_memory_equal(sv.data, "Hello World", 11);

  char* cstr = TPF_SBBuildCStr(sb);
  assert_non_null(cstr);
  assert_string_equal(cstr, "Hello World");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

static void test_appendf(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  assert_true(TPF_SBAppendf(sb, "%d + %d = %d", 1, 2, 3));
  char* cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "1 + 2 = 3");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

static void test_append_repeated(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  assert_true(TPF_SBAppendRepeated(sb, SVLIT("abc"), 3));
  char* cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "abcabcabc");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

static void test_case_modification(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  assert_true(TPF_SBAppendLowered(sb, SVLIT("HeLLo")));
  assert_true(TPF_SBAppend(sb, SVLIT(" ")));
  assert_true(TPF_SBAppendUppered(sb, SVLIT("world")));

  char* cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "hello WORLD");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

static void test_replaced(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  TPF_SV ori = SVLIT("banana");
  TPF_SV old_sv = SVLIT("a");
  TPF_SV new_sv = SVLIT("o");

  // Replace first 2 'a's with 'o's
  assert_true(TPF_SBAppendReplaced(sb, ori, old_sv, new_sv, 2));
  char* cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "bonona");
  SDL_free(cstr);

  TPF_DestroySB(sb);

  sb = TPF_CreateSB();
  // Replace all 'a's with 'o's
  assert_true(TPF_SBAppendReplacedAll(sb, ori, old_sv, new_sv));
  cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "bonono");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

static void test_joined(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  TPF_SV parts[] = {SVLIT("a"), SVLIT("b"), SVLIT("c")};
  assert_true(TPF_SBAppendJoined(sb, 3, parts, SVLIT(",")));

  char* cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "a,b,c");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

static void test_reserve(void** state) {
  (void)state;
  TPF_SB* sb = TPF_CreateSB();

  assert_true(TPF_SBReserve(sb, 1000));
  assert_true(TPF_SBAppend(sb, SVLIT("test")));
  assert_int_equal(TPF_SBTotalLen(sb), 4);

  char* cstr = TPF_SBBuildCStr(sb);
  assert_string_equal(cstr, "test");
  SDL_free(cstr);

  TPF_DestroySB(sb);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_basic_lifecycle),
      cmocka_unit_test(test_append_and_build),
      cmocka_unit_test(test_appendf),
      cmocka_unit_test(test_append_repeated),
      cmocka_unit_test(test_case_modification),
      cmocka_unit_test(test_replaced),
      cmocka_unit_test(test_joined),
      cmocka_unit_test(test_reserve),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
