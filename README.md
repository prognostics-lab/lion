[![Tests](https://github.com/case-ev/lion/actions/workflows/testing.yml/badge.svg)](https://github.com/case-ev/lion/actions/workflows/testing.yml)
[![Docs](https://github.com/case-ev/lion/actions/workflows/mdbook-pages.yml/badge.svg)](https://github.com/case-ev/lion/actions/workflows/mdbook-pages.yml)


```
 _ _
| (_)
| |_  ___  _ __
| | |/ _ \| '_ \
| | | (_) | | | |
|_|_|\___/|_| |_|

```

**Lithium-Ion battery simulator built in C using [GSL](https://www.gnu.org/software/gsl)**

[![Documentation](https://img.shields.io/badge/Documentation-000000?style=flat&logo=mdbook&logoColor=white)](https://case-ev.github.io/lion/)

<h2 align="center">Installation</h2>

The installation requires CMake, both for installing the native C library and the Python bindings. In Linux and macOS, lion should work without any major hassles, but for Windows it is recommended to use [WSL](https://learn.microsoft.com/en-us/windows/wsl/) to develop applications with the library, altough installation is supported natively using vcpkg.

>### Linux
For installation in Linux, the GSL library must be installed according to your distribution, as well as CMake. Then, simply run
```bash
make
sudo make install
```
to build and install the library, respectively. After this, the library files can be found in `/usr/lib`, and the headers in `/usr/include/lion/`.

Additionally, the Python bindings can be installed by calling
```bash
pip install -r requirements.txt
pip install .
```
after which the packages `lion` and `lion_utils` are installed.

>### macOS
Installation for macOS is equivalent to Linux. The recommended method to install GSL is using [homebrew](https://brew.sh/), by calling `brew install gsl`, but any other method should work as long as CMake is able to automatically find GSL.

>### Windows
Installation on Windows is only currently supported using [vcpkg](https://learn.microsoft.com/en-us/vcpkg/) to handle package installation and configuration of the toolchain. Having both vcpkg and CMake installed, the `VCPKG_ROOT` environment variable must point towards the directory in which vcpkg was installed. The recommended method to install the library is by directly installing the Python bindings, by calling
```bat
pip install -r requirements.txt
pip install .
```
which will compile the C libraries and leave them within the Python FFI, as well as install the Python bindings.

