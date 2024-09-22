"""Build the ffi"""

import os
import shutil
import sys
from pathlib import Path

import cffi

src_path = Path.joinpath(Path(__file__).parent, "pysrc")
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
    extra_link_args=["-Wl,-rpath=lib/:lib/debug/"],
)

# if __name__ == "__main__":
#     ffi_builder.compile(verbose=True)
