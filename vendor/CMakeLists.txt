# SDL3 3.4.12
if(TPF_USE_SYSTEM_SDL3)
  find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
else()
  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY  "https://github.com/libsdl-org/SDL.git"
    GIT_TAG         "release-3.4.12"
  )
  FetchContent_MakeAvailable(SDL3)
endif()

# TPF_cmake
FetchContent_Declare(
  TPF_cmake
  GIT_REPOSITORY  "https://github.com/tepachitos/TPF_cmake.git"
  GIT_TAG         "v1.0.0"
)
FetchContent_MakeAvailable(TPF_cmake)

# CMocka
if(TPF_STRINGS_TESTING)
  set(WITH_EXAMPLES OFF)
  set(UNIT_TESTING OFF)
  set(PICKY_DEVELOPER OFF)
  FetchContent_Declare(
    cmocka
    GIT_REPOSITORY  "https://gitlab.com/cmocka/cmocka"
    GIT_TAG         "stable-2.0"
  )
  FetchContent_MakeAvailable(cmocka)
endif()
