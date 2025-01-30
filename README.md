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

- [About](#about)
- [Installation](#installation)
    - [Linux](#linux)
    - [macOS](#macos)
    - [Windows](#windows)
    - [Documentation](#documentation)
- [License](#license)
- [References](#references)

<h2 id="about" align="center">About</h2>

lion simulates the electrothermal behavior of lithium-ion cells, taking into consideration effects such as temperature-induced capacity changes, ohmic and entropic heat generation, polarization resistance, etc. For more information, see [[1]](#ref1), which establishes the bases for this simulator (altough more advancements have been introduced as research continues).

<h2 id="installation" align="center">Installation</h2>

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
The recommended method of using lion in Windows is through [WSL](https://learn.microsoft.com/en-us/windows/wsl/), but native compilation is possible. Installation on Windows is only currently supported using [vcpkg](https://learn.microsoft.com/en-us/vcpkg/) to handle package installation and configuration of the toolchain. Having both vcpkg and CMake installed, with the `VCPKG_ROOT` environment variable pointing towards the directory in which vcpkg was installed, simply run
```bat
./make.bat
./make.bat install
```
which will configure and install the library, requesting administrator privileges for installation. However, if you don't intend to directly use the C library, the recommended method of installation is by simply running
```bat
pip install -r requirements.txt
pip install .
```
which will compile the C library and leave the binaries within the Python FFI, as well as install the Python bindings. Currently, the Python FFI only works with the binaries included within the package itself.

>### Documentation
The latest documentation is available at https://case-ev.github.io/lion/. Building the documentation requires [Doxygen](https://doxygen.nl/index.html) to compile the API reference into XML files, [moxygen](https://github.com/sourcey/moxygen) to transpile the XML files into Markdown files, and [mdBook](https://github.com/rust-lang/mdbook) to compile the Markdown documentation (which includes the API reference and more) into HTML files. With these three tools installed, run
```bash
make docs
```
to compile the docs, or
```bash
make docs-serve
```
to compile and open the docs in your browser.

---
<h2 id="license" align="center">License</h2>

lion is distributed under the MIT license. See `LICENSE` for more information.

<h2 id="references" align="center">References</h2>

<a name="ref1"></a>
**[1]** Allendes, C., Beltrán, A., García, J. E., Troncoso-Kurtovic, D., Masserano, B., Brito Schiele, B., Rivera, V., Jaramillo, F., Orchard, M. E., Silva, J. F., Rozas, H., & Rangarajan, S. (2024). Modeling and Simulation of Thermal Effects on Electrical Behavior in Lithium-Ion Cells. Annual Conference of the PHM Society, 16(1). https://doi.org/10.36001/phmconf.2024.v16i1.4080
