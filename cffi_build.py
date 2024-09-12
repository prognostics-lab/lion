"""Build the ffi"""

import os
import shutil
import sys
from pathlib import Path

import cffi

src_path = Path.joinpath(Path(__file__).parent, "pysrc")
os.environ["PYTHONPATH"] = str(src_path)
sys.path.append(str(src_path))
from lion_ffi.config import (CBIN_DEBUG_PATH, CBIN_RELEASE_PATH,
                             CLIB_DEBUG_PATH, CLIB_DLL_NAMES,
                             CLIB_RELEASE_PATH, CLIB_SO_NAMES, INCLUDE_DIRS,
                             SHLIB_PATH)
from lion_ffi.ffi import _app, _inputs, _params, _status, _vector


def _build_shared_lib() -> bool:
    """Build the underlying C library"""
    # TODO: Implement building the C library from Python
    return False


# Manually copy the corresponding shared libraries
def _get_shared_lib(flag=False):
    if os.path.exists(str(Path.joinpath(CLIB_RELEASE_PATH, CLIB_SO_NAMES[0]))):
        path = CLIB_RELEASE_PATH
        files = CLIB_SO_NAMES
    elif os.path.exists(str(Path.joinpath(CLIB_DEBUG_PATH, CLIB_SO_NAMES[0]))):
        path = CLIB_DEBUG_PATH
        files = CLIB_SO_NAMES
    elif os.path.exists(str(Path.joinpath(CBIN_RELEASE_PATH, CLIB_DLL_NAMES[0]))):
        path = CBIN_RELEASE_PATH
        files = CLIB_DLL_NAMES
    elif os.path.exists(str(Path.joinpath(CBIN_DEBUG_PATH, CLIB_DLL_NAMES[0]))):
        path = CBIN_DEBUG_PATH
        files = CLIB_DLL_NAMES
    else:
        if flag:
            return None
        if not _build_shared_lib():
            return None
        return _get_shared_lib(True)
    return path, files


ret = _get_shared_lib()
if ret is None:
    print("ERROR: Could not find shared library files", file=sys.stderr)
    sys.exit(1)
path, files = ret
for f in files:
    shutil.copyfile(str(Path.joinpath(path, f)), str(Path.joinpath(SHLIB_PATH, f)))


# Parameters for the FFI

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
"""

FFI_CDEF = f"""
// Typedefs
{LIB_TYPEDEF}
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

for i, line in enumerate(FFI_CDEF.splitlines()):
    print(f"{i:>4} | {line}")

LIB_SOURCE = """
#include <lion/lion.h>
"""

# Builder for the FFI

ffi_builder = cffi.FFI()

ffi_builder.cdef(FFI_CDEF)
ffi_builder.set_source(
    "_lion",
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
