cmake_minimum_required(VERSION 3.21)

# Only set the cxx_standard if it is not set by someone else
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the project name and language
project(
  moncfg
  VERSION 0.0.1
  DESCRIPTION "MonitorConfigurator"
  HOMEPAGE_URL "https://www.github.com/michieltukker/???"
  LANGUAGES CXX C)

include(cmake/PreventInSourceBuilds.cmake)
include(cmake/ProjectOptions.cmake)

moncfg_setup_options()
moncfg_global_options()

include(cmake/Dependencies.cmake)
moncfg_setup_dependencies()

moncfg_local_options()

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")
string(
  SUBSTRING "${GIT_SHA}"
            0
            8
            GIT_SHORT_SHA)

target_compile_features(moncfg_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(moncfg::moncfg_options ALIAS moncfg_options)
add_library(moncfg::moncfg_warnings ALIAS moncfg_warnings)

# configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the src:
add_subdirectory(src)
# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Adding the tests:
include(CTest)
if(BUILD_TESTING)
  message(AUTHOR_WARNING "Building Tests. Be sure to check out test/constexpr_tests.cpp for constexpr testing")
  add_subdirectory(test)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()