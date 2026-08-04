set(TPF_MODULE TPF_strings)
add_library(${TPF_MODULE} STATIC)
target_sources(${TPF_MODULE} PRIVATE
  strbuild.c
  strview.c
  utf8.c
)
target_link_libraries(${TPF_MODULE} PRIVATE
  SDL3
)
target_include_directories(${TPF_MODULE} PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
)

if(TPF_STRINGS_TESTING)
  setup_test(
    NAME strbuild
    LIBS SDL3 ${TPF_MODULE}
    INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
  )
  add_memcheck_target(strbuild_test)

  setup_test(
    NAME strview
    LIBS SDL3 ${TPF_MODULE}
    INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
  )
  add_memcheck_target(strview_test)

  setup_test(
    NAME utf8
    LIBS SDL3 ${TPF_MODULE}
    INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include"
  )
  add_memcheck_target(utf8_test)
endif()

unset(TPF_MODULE CACHE)
