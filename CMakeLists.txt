cmake_minimum_required(VERSION 3.23)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

project(lion VERSION 0.1)
set(CMAKE_DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/dependencies)

# Necessary libraries
find_package(GSL REQUIRED)

# Output for files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/pdb)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/pdb)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib/debug)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/pdb/debug)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/pdb/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/pdb)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/pdb)

# Directory of the code
set(PROJECT_SOURCE_DIR_LOCATION ${CMAKE_SOURCE_DIR}/src)

# Names for the different libraries in the project
set(PROJECT_APP_NAME ${PROJECT_NAME}_app)
set(PROJECT_MATH_NAME ${PROJECT_NAME}_math)
set(PROJECT_UTILS_NAME ${PROJECT_NAME}_utils)
set(PROJECT_HEADERS ${CMAKE_SOURCE_DIR}/include)

option(LION_BUILD_EXAMPLES "Build the examples that come with the package." OFF)
option(LION_BUILD_TESTS "Build the tests that come with the package." OFF)
option(LION_INSTALL "Install target." OFF)

# Constants for the project
if(MSVC)
  set(PROJECT_COMPILE_OPTIONS /WX /W4)
else()
  set(PROJECT_COMPILE_OPTIONS
      -Wall
      -Wextra
      -Werror
      -Wno-unused-parameter
      -Wno-unused-function
      -Wconversion
      -Wsign-conversion
      -pedantic
      -pedantic-errors)
endif()

add_compile_definitions(LOG_USE_COLOR)
add_compile_definitions(LION_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}/")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  add_compile_definitions(LION_BUILD_TYPE_RELEASE)
  add_compile_definitions(LION_DISABLE_LOGGING)
else()
  add_compile_definitions(LION_BUILD_TYPE_DEBUG)
endif()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# Subdirectories

add_subdirectory(dependencies)
add_subdirectory(src)

if(${LION_BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()

if(${LION_BUILD_TESTS})
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
