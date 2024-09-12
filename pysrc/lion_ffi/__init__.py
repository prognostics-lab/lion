"""FFI for the lion C functions"""

import os
from pathlib import Path

lib_path = str(Path(__file__).parent)
try:
    new_path = f"{lib_path}" + f"{os.environ['LD_LIBRARY_PATH']}"
    os.environ["LD_LIBRARY_PATH"] = new_path
except KeyError:
    os.environ["LD_LIBRARY_PATH"] = lib_path

# Add the dll directory to the trusted path
_WINDOWS_NAME = "nt"
if os.name == _WINDOWS_NAME:
    os.add_dll_directory(os.environ["LD_LIBRARY_PATH"])
