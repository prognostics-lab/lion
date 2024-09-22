import os
import pathlib

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])


class cmake_ext(build_ext):

    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        cwd = pathlib.Path().absolute()

        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        extdir = pathlib.Path(self.get_ext_fullpath(ext.name))
        extdir.mkdir(parents=True, exist_ok=True)

        # example of cmake args
        config = "Debug" if self.debug else "Release"
        cmake_args = [
            "-DCMAKE_BUILD_TYPE=" + config,
            "-DBUILD_SHARED_LIBS=ON",
        ]

        # example of build args
        build_args = [
            "--config",
            config,
            "--",
            "-j4",
        ]

        self.spawn(["cmake", "-S", ".", "-B", str(build_temp)] + cmake_args)
        if not self.dry_run:
            self.spawn(["cmake", "--build", str(build_temp)] + build_args)

        # _get_ffi_builder().compile(verbose=True)


# TODO: Set version dynamically
if __name__ == "__main__":
    setup(
        cffi_modules=["cffi_build.py:ffi_builder"],
        ext_modules=[CMakeExtension("lion")],
        cmdclass={
            "build_ext": cmake_ext,
        },
    )
