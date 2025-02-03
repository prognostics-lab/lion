# Installation > Linux
## C library

For building in Linux, the GSL library must be installed according to your distribution: for some common distributions, the corresponding instruction would be
```bash
sudo apt install libgsl-dev  # Ubuntu
sudo dnf install gsl         # Fedora
sudo pacman -S gsl           # Arch
```
lion uses [CMake](https://cmake.org/) as the build system, so you also need to have it installed. Then, simply run
```bash
make
sudo make install
```
to build and install the library, respectively. After this, the library files can be found in `/usr/lib`, and the headers in `/usr/include/lion/`.

## Python bindings

Additionally, once the C library is installed the Python bindings can be installed by calling
```bash
pip install -r requirements.txt
pip install .
```
after which the packages `lion` and `lion_utils` are installed.
