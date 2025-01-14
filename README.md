# lion

Lithium-Ion battery simulator built in C using GSL.

[![Tests](https://github.com/case-ev/lion/actions/workflows/testing.yml/badge.svg)](https://github.com/case-ev/lion/actions/workflows/testing.yml)
[![Docs](https://github.com/case-ev/lion/actions/workflows/mdbook-pages.yml/badge.svg)](https://github.com/case-ev/lion/actions/workflows/mdbook-pages.yml)

# Installation
The installation requires CMake, both for installing the native C library and the FFI to use *lion* with Python.
## Linux
For installation in Linux, the *gsl* library must be installed according to your distribution, as well as *CMake*. Then, you can run the bash script `sys/build.sh` to build and install the library: the recommended script call is
```bash
sys/build.sh -trhi
```
where `-t` builds and calls the tests using *CTest*, `-r` compiles the library in release mode, `-h` builds *lion* as shared libraries, and `-i` installs the library to the `/usr/` directory. After this, the library files can be found in `/usr/lib`, and the headers in `/usr/include/lion/`.

Additionally, the Python bindings can be installed by calling
```bash
pip install .
```
which requires the `cmake` and `cffi` packages to build.

## Windows
<!-- Installation on Windows is only currently supported using (*vcpkg*)[https://learn.microsoft.com/en-us/vcpkg/] to handle package installation and compilation of the toolchain. Having both *vcpkg* and *CMake* installed, the `VCPKG_ROOT` environment variable must point towards the directory in which *vcpkg* was installed, after which you should call -->
<!-- ```bat -->
<!-- cmake -S . -B build -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake -->
<!-- ``` -->
<!-- to setup the build, and -->
<!-- ```bat -->
<!-- cmake --build build --config Release -->
<!-- ``` -->
<!-- to build the project. -->

Installation on Windows is only currently supported using (*vcpkg*)[https://learn.microsoft.com/en-us/vcpkg/] to handle package installation and compilation of the toolchain. Having both *vcpkg* and *CMake* installed, the `VCPKG_ROOT` environment variable must point towards the directory in which *vcpkg* was installed. The recommended method to install the library is by directly installing the FFI by calling
```bat
pip install .
```
which will locally install the C libraries and leave them within the Python FFI.

## macOS
Installation in macOS requires having (*homebrew)[https://brew.sh/] installed.
