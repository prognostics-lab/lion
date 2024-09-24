import os
import sys
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
        print(f"Build temp {build_temp.resolve()}")
        extdir = pathlib.Path(self.get_ext_fullpath(ext.name))
        extdir.mkdir(parents=True, exist_ok=True)
        print(f"Extension dir {extdir.resolve()}")

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
            self.spawn(["cmake", "--install", str(build_temp)])

        # get_ffi_builder().compile(verbose=True)


# TODO: Set version dynamically
if __name__ == "__main__":
    setup(
        # ext_modules=[CMakeExtension("lion")],
        # cmdclass={
        #     "build_ext": cmake_ext,
        # },
        cffi_modules=["pysrc/lion_ffi/cffi_build.py:ffi_builder"],
    )
