import os
import sys
import pathlib
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
from lion_ffi.ffi import _app, _inputs, _params, _status, _vector


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
{_inputs.CTYPEDEF}
{_app.CTYPEDEF}
{_vector.CTYPEDEF}

// Function definitions
{_status.CDEF}
{_params.CDEF}
{_inputs.CDEF}
{_vector.CDEF}
{_app.CDEF}
"""

# for i, line in enumerate(FFI_CDEF.splitlines()):
#     print(f"{i:>4} | {line}")

LIB_SOURCE = """
#include <lion/lion.h>
#include <lionu/log.h>
"""

# Builder for the FFI

ffi_builder = cffi.FFI()

ffi_builder.cdef(FFI_CDEF)
ffi_builder.set_source(
    "lion._lion",
    LIB_SOURCE,
    libraries=["lion_app", "lion_math", "lion_utils"],
    library_dirs=[
        str(CBIN_DEBUG_PATH),
        str(CLIB_DEBUG_PATH),
        str(CBIN_RELEASE_PATH),
        str(CLIB_RELEASE_PATH),
    ],
    include_dirs=INCLUDE_DIRS,
    extra_link_args=["-Wl,-rpath=/usr/lib:lib/:lib/debug/:/opt/homebrew/lib"],
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

        # TODO: Fix the bug where it has to be installed twice to work
        print("=== Building C FFI ===")
        ffi_builder.compile("pysrc", verbose=True)



# TODO: Set version dynamically
if __name__ == "__main__":
    setup(
        ext_modules=[CMakeExtension("lion")],
        cmdclass={
            "build_ext": cmake_ext,
        },
        # cffi_modules=["pysrc/lion_ffi/cffi_build.py:ffi_builder"],
    )
