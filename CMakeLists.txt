cmake_minimum_required(VERSION 3.23)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

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

# Project setup
file(READ "${CMAKE_SOURCE_DIR}/VERSION" _version)
string(REGEX MATCH "([0-9]*).([0-9]*).([0-9]*)" _ ${_version})
set(_ver_major ${CMAKE_MATCH_1})
set(_ver_minor ${CMAKE_MATCH_2})
set(_ver_patch ${CMAKE_MATCH_3})
project(lion VERSION ${_ver_major}.${_ver_minor}.${_ver_patch})
message(STATUS "Project version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(CMAKE_DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/dependencies)
include(GNUInstallDirs)

# Necessary libraries
if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    message(STATUS "Toolchain: ${CMAKE_TOOLCHAIN_FILE}")
    include(cmake/Vcpkg.cmake)
endif()
find_package(GSL REQUIRED)

# Outputs for files
include(cmake/Outputs.cmake)

# Directory of the code
set(PROJECT_SOURCE_DIR_LOCATION ${CMAKE_SOURCE_DIR}/src)

# Names for the different libraries in the project
set(PROJECT_APP_NAME ${PROJECT_NAME}_app)
set(PROJECT_CPP_NAME ${PROJECT_NAME}_cpp)
set(PROJECT_MATH_NAME ${PROJECT_NAME}_math)
set(PROJECT_UTILS_NAME ${PROJECT_NAME}_utils)
set(PROJECT_HEADERS ${CMAKE_SOURCE_DIR}/include)

include(cmake/StandardOptions.cmake)
option(LION_BUILD_EXAMPLES "Build the examples that come with the package." OFF)
option(LION_BUILD_TESTS "Build the tests that come with the package." OFF)

# Constants for the project
add_compile_definitions(LOG_USE_COLOR)
add_compile_definitions(LION_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}/")
add_compile_definitions(LION_ENGINE_VERSION_MAJOR="${PROJECT_VERSION_MAJOR}")
add_compile_definitions(LION_ENGINE_VERSION_MINOR="${PROJECT_VERSION_MINOR}")
add_compile_definitions(LION_ENGINE_VERSION_PATCH="${PROJECT_VERSION_PATCH}")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  add_compile_definitions(LION_BUILD_TYPE_RELEASE)
else()
  add_compile_definitions(LION_BUILD_TYPE_DEBUG)
endif()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# Fix for building in Windows
# TODO: See if it is necessary
set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_SHARED_MODULE_SUFFIX}")

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

message(STATUS "Installation directories")
message(STATUS "Binaries  : ${CMAKE_INSTALL_BINDIR}")
message(STATUS "Libraries : ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "Archives  : ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "Headers   : ${CMAKE_INSTALL_INCLUDEDIR}")

# libraries
install(TARGETS ${PROJECT_APP_NAME} ${PROJECT_CPP_NAME} ${PROJECT_MATH_NAME} ${PROJECT_UTILS_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# headers
install(DIRECTORY ${PROJECT_HEADERS}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# TODO: Add creation of a `.cmake` file to find the library
