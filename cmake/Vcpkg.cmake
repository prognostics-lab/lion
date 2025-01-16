# TODO: Analyze the proper way to handle this scenario
if(NOT EXISTS "${CMAKE_BINARY_DIR}/vcpkg.cmake")
    message(
    STATUS
        "Downloading `vcpkg.cmake` from https://github.com/microsoft/vcpkg..."
    )
    file(DOWNLOAD "https://github.com/microsoft/vcpkg/raw/master/scripts/buildsystems/vcpkg.cmake"
    "${CMAKE_BINARY_DIR}/vcpkg.cmake"
    )
    message(STATUS "Vcpkg config downloaded succesfully.")
endif()

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" "${CMAKE_BINARY_DIR}/vcpkg.cmake")
