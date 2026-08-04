set(TPF_MODULE TPF_strings)
if(TPF_STRINGS_STATIC)
  add_library(${TPF_MODULE} STATIC)
else()
  add_library(${TPF_MODULE} SHARED)
endif()

# Library
target_sources(${TPF_MODULE} PRIVATE
  strbuild.c
  strview.c
  utf8.c
)
target_link_libraries(${TPF_MODULE} PRIVATE
  SDL3
)
target_include_directories(${TPF_MODULE} PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
)

# Public headers
file(GLOB PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/TPF1/*.h")
message(FATAL_ERROR ${PUBLIC_HEADERS})

# Docs
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_NAME    "TPF_strings")
  set(DOXYGEN_PROJECT_NUMBER  "1.0.0")
  set(DOXYGEN_PROJECT_BRIEF   "TPF Strings utilities")
  set(DOXYGEN_GENERATE_HTML   YES)
  set(DOXYGEN_GENERATE_MAN    NO)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs")
  doxygen_add_docs(${TPF_MODULE}_docs "${PROJECT_SOURCE_DIR}/include")
endif()

# Install targets
set_target_properties(${TPF_MODULE}
    PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)
install(TARGETS ${TPF_MODULE}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# Unit testing
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
