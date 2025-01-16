<p align="center">

```
 _ _
| (_)
| |_  ___  _ __
| | |/ _ \| '_ \
| | | (_) | | | |
|_|_|\___/|_| |_|

```


**Lithium-Ion battery simulator built in C using [GSL](https://www.gnu.org/software/gsl)**
</p>
[![Tests](https://github.com/case-ev/lion/actions/workflows/testing.yml/badge.svg)](https://github.com/case-ev/lion/actions/workflows/testing.yml)
[![Docs](https://github.com/case-ev/lion/actions/workflows/mdbook-pages.yml/badge.svg)](https://github.com/case-ev/lion/actions/workflows/mdbook-pages.yml)

# Installation
The installation requires CMake, both for installing the native C library and the FFI to use lion with Python.
## Linux
For installation in Linux, the GSL library must be installed according to your distribution, as well as CMake. Then, you can run the bash script `sys/build.sh` to build and install the library: the recommended script call is
```bash
sys/build.sh -trhi
```
where `-t` builds and calls the tests using CTest, `-r` compiles the library in release mode, `-h` builds lion as shared libraries, and `-i` installs the library to the `/usr/` directory. After this, the library files can be found in `/usr/lib`, and the headers in `/usr/include/lion/`.

Additionally, the Python bindings can be installed by calling
```bash
pip install -r requirements.txt
pip install .
```
after which the packages `lion` and `lion_utils` are installed.

## Windows
Installation on Windows is only currently supported using [vcpkg](https://learn.microsoft.com/en-us/vcpkg/) to handle package installation and compilation of the toolchain. Having both vcpkg and CMake installed, the `VCPKG_ROOT` environment variable must point towards the directory in which vcpkg was installed. The recommended method to install the library is by directly installing the Python FFI by calling
```bat
pip install -r requirements.txt
pip install .
```
which will locally install the C libraries and leave them within the Python FFI, as well as install the Python bindings.

## macOS
The installation steps for macOS is equivalent to Linux. The recommended method to install GSL is using [homebrew](https://brew.sh/), by calling `brew install gsl`, but any other method should work as long as CMake is able to automatically find GSL.
