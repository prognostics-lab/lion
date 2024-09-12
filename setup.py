from setuptools import setup

# The setup assumes the C library has been built and the .dll/.so files are located
# in "bin" for Windows and "lib" for Linux

if __name__ == "__main__":
    setup(
        setup_requires=["cffi>=1.0.0"],
        cffi_modules=["cffi_build.py:ffi_builder"],
        install_requires=["cffi>=1.0.0"],
    )
