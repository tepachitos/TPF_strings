cmake_minimum_required(VERSION 3.22)
project(TPF_utf8
  VERSION "1.0.0"
  LANGUAGES C
  DESCRIPTION "TPF Generic UTF8 utilities")

# CMake configuration
option(TPF_ENABLE_TESTS "Enable tests for TPF modules" ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(PROJECT_IS_TOP_LEVEL)
  # CMake internals
  include(vendor/TPF_cmake/testing.cmake)
  include(vendor/TPF_cmake/memcheck.cmake)
  if(TPF_ENABLE_TESTS)
    enable_testing()
  endif()

  # System packages
  find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

  # Vendor dependencies
  add_subdirectory(vendor)
endif()

# This module
add_library(TPF_utf8 STATIC)
target_sources(TPF_utf8 PRIVATE utf8.c)
target_link_libraries(TPF_utf8 PRIVATE SDL3::SDL3)
target_include_directories(TPF_utf8 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(TPF_utf8 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Unit testing
if(TPF_ENABLE_TESTS)
  setup_test(NAME utf8 LIBS SDL3::SDL3 TPF_utf8)
  add_memcheck_target(utf8_test)
endif()
