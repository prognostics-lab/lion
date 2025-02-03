# Installation > Windows
The recommended method of using lion in Windows is through [WSL](https://learn.microsoft.com/en-us/windows/wsl/), for which the [Linux](linux.md) instructions should be followed, but native compilation is possible.

## C library

Installation on Windows is only currently supported using [vcpkg](https://learn.microsoft.com/en-us/vcpkg/) to handle package installation and configuration of the toolchain. Having both vcpkg and CMake installed, the `VCPKG_ROOT` environment variable must be pointing towards the directory in which vcpkg was installed: then, simply run
```bat
./make.bat
./make.bat install
```
which will configure and install the library, requesting administrator privileges for installation.

## Python bindings
In order to install the Python bindings, it is *not* necessary to have the C library installed, as installing the Python library will build the binaries and ship them within the source files: thus, if you only intend to use lion as a Python library, it is recommended to only install the bindings. Having CMake and vcpkg installed with the `VCPKG_ROOT` environment variable properly configured, simply run
```bat
pip install -r requirements.txt
pip install .
```
which will compile the C library and leave the binaries within the Python FFI, as well as install the Python bindings. Currently, the Python FFI only works with the binaries included within the package itself.

