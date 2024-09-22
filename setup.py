from setuptools import setup


# TODO: Set version dynamically
if __name__ == "__main__":
    setup(
        cffi_modules=["cffi_build.py:ffi_builder"],
    )
