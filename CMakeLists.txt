cmake_minimum_required(VERSION 3.22)
project(TPF_strings
  VERSION "1.0.0"
  LANGUAGES C
  DESCRIPTION "TPF String utilities")

# cmake options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# cmake utilities
include(FetchContent)

# project options
option(TPF_USE_SYSTEM_SDL3 "TPF: Use system SDL3" ON)
option(TPF_STRINGS_STATIC "TPF_strings: Build static library" ON)
option(TPF_STRINGS_TESTING "TPF_strings: Enable unit testing" ON)
if(TPF_STRINGS_TESTING)
  enable_testing()
endif()

# subdirectories
if(PROJECT_IS_TOP_LEVEL)
  add_subdirectory(vendor)
endif()

add_subdirectory(src)
