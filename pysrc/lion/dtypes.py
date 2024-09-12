from typing import Any

import numpy as np


class DataType:
    def __init__(self, size_bytes, name, long_name, np_equiv=None) -> None:
        self.size = size_bytes
        self.name = name
        self.long_name = long_name
        self.np = np_equiv

    @property
    def size_bits(self) -> int:
        return self.size * 8

    def __str__(self) -> str:
        return self.name

    def __repr__(self) -> str:
        return self.long_name


UINT8 = DataType(1, "u8", "uint8_t", np.uint8)
UINT16 = DataType(2, "u16", "uint16_t", np.uint16)
UINT32 = DataType(4, "u32", "uint32_t", np.uint32)
UINT64 = DataType(8, "u64", "uint64_t", np.uint64)

INT8 = DataType(1, "i8", "int8_t", np.int8)
INT16 = DataType(2, "i16", "int16_t", np.int16)
INT32 = DataType(4, "i32", "int32_t", np.int32)
INT64 = DataType(8, "i64", "int64_t", np.int64)

FLOAT32 = DataType(4, "f32", "float", np.float32)
FLOAT64 = DataType(8, "f64", "double", np.float64)

_NP_TYPES = {
    "uint8": UINT8,
    "uint16": UINT16,
    "uint32": UINT32,
    "uint64": UINT64,
    "int8": INT8,
    "int16": INT16,
    "int32": INT32,
    "int64": INT64,
    "float32": FLOAT32,
    "float64": FLOAT64,
}


def get_ctype(value: Any) -> DataType | None:
    if isinstance(value, int):
        return INT32
    elif isinstance(value, float):
        return FLOAT64
    return None
