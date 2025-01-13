cmake_minimum_required(VERSION 3.23)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler and generator configurations
# set(CMAKE_C_COMPILER /usr/bin/clang)
# set(CMAKE_CXX_COMPILER /usr/bin/clang++)
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
project(lion VERSION 0.2.0)
set(CMAKE_DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/dependencies)

# Necessary libraries
if(MSVC)
    # TODO: Properly configure building on Windows
    # TODO: Add a way to set the path to the GSL installation
    # set(GSL_ROOT_DIR "C:/Program Files (x86)/GSL")
    # set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    message(STATUS "Toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()
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
set(PROJECT_CPP_NAME ${PROJECT_NAME}_cpp)
set(PROJECT_MATH_NAME ${PROJECT_NAME}_math)
set(PROJECT_UTILS_NAME ${PROJECT_NAME}_utils)
set(PROJECT_HEADERS ${CMAKE_SOURCE_DIR}/include)

option(LION_BUILD_EXAMPLES "Build the examples that come with the package." OFF)
option(LION_BUILD_TESTS "Build the tests that come with the package." OFF)
option(LION_INSTALL "Install target." OFF)

# Constants for the project
add_compile_definitions(LOG_USE_COLOR)
add_compile_definitions(LION_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}/")
add_compile_definitions(LION_ENGINE_VERSION_MAJOR="${PROJECT_VERSION_MAJOR}")
add_compile_definitions(LION_ENGINE_VERSION_MINOR="${PROJECT_VERSION_MINOR}")
add_compile_definitions(LION_ENGINE_VERSION_PATCH="${PROJECT_VERSION_PATCH}")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  add_compile_definitions(LION_BUILD_TYPE_RELEASE)
  # add_compile_definitions(LION_DISABLE_LOGGING)
else()
  add_compile_definitions(LION_BUILD_TYPE_DEBUG)
endif()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# Fix for building in Windows
set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_SHARED_MODULE_SUFFIX}")
# if(MSVC)
# configure_file(
#   ${CMAKE_CURRENT_SOURCE_DIR}/FixBundle.cmake.in
#   ${CMAKE_CURRENT_BINARY_DIR}/FixBundle.cmake
#   @ONLY
#   )
# endif()

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

if(${LION_INSTALL})
  # libraries
  install(TARGETS ${PROJECT_APP_NAME} ${PROJECT_CPP_NAME} ${PROJECT_MATH_NAME} ${PROJECT_UTILS_NAME}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      # RUNTIME_DEPENDENCIES
      # PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
      # POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
  )

  # headers
  file(GLOB BASE_HEADERS ${PROJECT_HEADERS}/lion/*.h)
  file(GLOB BASE_SOLVER_HEADERS ${PROJECT_HEADERS}/lion/solver/*.h)
  file(GLOB CPP_HEADERS ${PROJECT_HEADERS}/lionpp/*.hpp)
  file(GLOB UTILS_HEADERS ${PROJECT_HEADERS}/lionu/*.h)
  file(GLOB UTILS_FUZZY_HEADERS ${PROJECT_HEADERS}/lionu/fuzzy/*.h)
  install(FILES ${BASE_HEADERS} DESTINATION         ${CMAKE_INSTALL_INCLUDEDIR}/lion)
  install(FILES ${BASE_SOLVER_HEADERS} DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/lion/solver)
  install(FILES ${CPP_HEADERS} DESTINATION          ${CMAKE_INSTALL_INCLUDEDIR}/lionpp)
  install(FILES ${UTILS_HEADERS} DESTINATION        ${CMAKE_INSTALL_INCLUDEDIR}/lionu)
  install(FILES ${UTILS_FUZZY_HEADERS} DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}/lionu/fuzzy)

  # if(MSVC)
  #   install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/FixBundle.cmake)
  # endif()
endif()
