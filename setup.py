import os
import sys
import pathlib
import shutil
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

import cffi

src_path = pathlib.Path.joinpath(pathlib.Path(__file__).parent, "pysrc")
os.environ["PYTHONPATH"] = str(src_path)
sys.path.append(str(src_path))
from lion_ffi.config import (
    CBIN_DEBUG_PATH,
    CBIN_RELEASE_PATH,
    CLIB_DEBUG_PATH,
    CLIB_RELEASE_PATH,
    INCLUDE_DIRS,
)
from lion_ffi.ffi import _sim, _params, _status, _vector, _names


LIB_TYPEDEF = """
typedef struct lion_mf_sigmoid_params {
  double a;
  double c;
} lion_mf_sigmoid_params_t;

typedef struct lion_mf_gaussian_params {
  double mean;
  double sigma;
} lion_mf_gaussian_params_t;

typedef struct gsl_odeiv2_system gsl_odeiv2_system;
typedef struct gsl_odeiv2_driver gsl_odeiv2_driver;
typedef struct gsl_odeiv2_step_type gsl_odeiv2_step_type;
typedef struct gsl_min_fminimizer gsl_min_fminimizer;
typedef struct gsl_min_fminimizer_type gsl_min_fminimizer_type;

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };
"""

FFI_CDEF = f"""
{LIB_TYPEDEF}

// Typedefs
{_status.CTYPEDEF}
{_params.CTYPEDEF}
{_sim.CTYPEDEF}
{_names.CTYPEDEF}
{_vector.CTYPEDEF}

// Function definitions
{_status.CDEF}
{_params.CDEF}
{_sim.CDEF}
{_names.CDEF}
{_vector.CDEF}
"""

# for i, line in enumerate(FFI_CDEF.splitlines()):
#     print(f"{i:>4} | {line}")

LIB_SOURCE = """
#include <lion/lion.h>
#include <lionu/log.h>
"""

# Builder for the FFI

LINK_ARGS = [
    "-Wl,-rpath,/usr/lib",
    "-Wl,-rpath,bin/",
    "-Wl,-rpath,bin/debug/",
    "-Wl,-rpath,lib/",
    "-Wl,-rpath,lib/debug/",
    "-Wl,-rpath,/opt/homebrew/lib",
]

ffi_builder = cffi.FFI()

ffi_builder.cdef(FFI_CDEF)
ffi_builder.set_source(
    "lion._lion",
    LIB_SOURCE,
    libraries=["lion_sim", "lion_math", "lion_utils"],
    library_dirs=[
        str(CBIN_DEBUG_PATH),
        str(CLIB_DEBUG_PATH),
        str(CBIN_RELEASE_PATH),
        str(CLIB_RELEASE_PATH),
        os.path.join(os.sep, "opt", "homebrew", "lib")
    ],
    include_dirs=INCLUDE_DIRS,
    extra_link_args=LINK_ARGS,
)


class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])


class cmake_ext(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        # print("=== Building C FFI ===")
        # ffi_builder.compile("pysrc", verbose=True)

        print("=== Building CMake project ===")
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
        if sys.platform == "win32":
            toolchain = os.path.join(
                os.environ["VCPKG_ROOT"], "scripts", "buildsystems", "vcpkg.cmake"
            )
            cmake_args = [
                *cmake_args,
                f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
            ]

        # example of build args
        build_args = [
            "--config",
            config,
        ]

        self.spawn(["cmake", "-S", ".", "-B", str(build_temp)] + cmake_args)
        if not self.dry_run:
            self.spawn(["cmake", "--build", str(build_temp)] + build_args)
            # self.spawn(["cmake", "--install", str(build_temp)])

        # TODO: Fix the bug where it has to be installed twice to work
        print("=== Building C FFI ===")
        ffi_builder.compile("pysrc", verbose=True)

        if sys.platform == "win32":
            print("=== Moving extension files ===")
            for f in os.listdir(os.path.join(cwd, "pysrc", "Release", "lion")):
                print(f)
                shutil.move(
                    os.path.join(cwd, "pysrc", "Release", "lion", f),
                    os.path.join(cwd, "pysrc", "lion", f),
                )

            print("=== Moving dll files ===")
            for f in os.listdir(os.path.join(cwd, "bin")):
                if f.endswith("dll"):
                    print(f)
                    shutil.move(
                        os.path.join(cwd, "bin", f),
                        os.path.join(cwd, "pysrc", "lion", f),
                    )


if __name__ == "__main__":
    setup(
        ext_modules=[CMakeExtension("lion")],
        cmdclass={
            "build_ext": cmake_ext,
        },
    )
