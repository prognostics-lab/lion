from collections.abc import Iterable, Iterator
from enum import Enum
from functools import singledispatchmethod
from numbers import Real
from typing import List, Self

import lion_ffi as _
import numpy as np
import pandas as pd
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion import dtypes
from lion.exceptions import LionException
from lion.status import ffi_call
from lion_utils.logger import LOGGER


Vectorizable = List | np.ndarray | Iterator | Iterable | Real


class Vector:
    """Vector of data allocated in a given device"""

    __slots__ = ("_cdata", "_app", "_dtype", "_dsize", "_index")

    def __init__(self, dtype: dtypes.DataType, app=None):
        if app is None:
            self._app = ffi.NULL
        else:
            self._app = app._cdata
        self._cdata = ffi.new("lion_vector_t *")
        self._dtype = dtype
        self._dsize = self._dtype.size

    @classmethod
    def empty(cls, dtype: dtypes.DataType):
        """Create an empty vector"""
        buf = cls(dtype)
        ffi_call(
            _lionl.lion_vector_new(buf._app, buf._dsize, buf._cdata),
            "Failed creating empty vector",
        )
        return buf

    @classmethod
    def zero(cls, size: int, dtype: dtypes.DataType):
        """Create a zero-initialized vector"""
        buf = cls(dtype)
        ffi_call(
            _lionl.lion_vector_zero(buf._app, size, buf._dsize, buf._cdata),
            "Failed creating zero initialized vector",
        )
        return buf

    @classmethod
    def with_capacity(
        cls,
        capacity: int,
        dtype: dtypes.DataType,
    ):
        """Create an empty vector with a given capacity"""
        buf = cls(dtype)
        ffi_call(
            _lionl.lion_vector_with_capacity(
                buf._app, capacity, buf._dsize, buf._cdata
            ),
            f"Failed creating vector with capacity {capacity}",
        )
        return buf

    @classmethod
    def from_list(
        cls,
        target: List,
        dtype: dtypes.DataType | None = None,
    ):
        """Create a vector from a list, using size as capacity"""
        LOGGER.debug("Creating from list")
        # TODO: Fix the bug that happens when the first element is less general than the rest
        if dtype is None:
            if target == []:
                LOGGER.error("Found empty list, raising exception")
                raise ValueError(
                    "Expected non-empty empty list (see `Vector.empty` for empty initialization)"
                )
            dtype = dtypes.get_ctype(target[0])
            if dtype is None:
                raise TypeError(
                    f"Conversion of type '{
                        type(target[0]).__name__}' not implemented"
                )
        buf = cls(dtype)
        size = len(target)
        val = ffi.new(f"{dtype.long_name}[]", target)
        ffi_call(
            _lionl.lion_vector_from_array(buf._app, val, size, dtype.size, buf._cdata),
            "Failed creating vector from list",
        )
        return buf

    @classmethod
    def from_numpy(
        cls,
        target: np.ndarray,
        dtype: dtypes.DataType | None = None,
    ):
        """Create a vector from a numpy array, using size as capacity"""
        # TODO: Determine a more efficient way of creating from numpy array
        LOGGER.debug("Creating from numpy array")
        if dtype is None:
            dtype = dtypes._NP_TYPES.get(target.dtype.name)
            if dtype is None:
                raise TypeError(
                    f"Conversion of type 'np.{
                        target.dtype.name}' not implemented"
                )
        buf = cls(dtype)
        size = len(target.flatten())
        val = ffi.new(f"{dtype.long_name}[]", list(target.flatten()))
        ffi_call(
            _lionl.lion_vector_from_array(buf._app, val, size, dtype.size, buf._cdata),
            "Failed creating vector from numpy array",
        )
        return buf

    @classmethod
    def from_iter(
        cls,
        target: Iterator,
        dtype: dtypes.DataType | None = None,
    ):
        """Create a vector from an iterator"""
        LOGGER.debug("Creating from iterator")
        vals = list(target)
        if dtype is None:
            if vals == []:
                LOGGER.error("Found empty iterator, raising exception")
                raise ValueError(
                    "Expected non-empty empty iterator (see `Vector.empty` for empty initialization)"
                )
            dtype = dtypes.get_ctype(vals[0])
            if dtype is None:
                raise TypeError(
                    f"Conversion of type '{
                        type(vals[0]).__name__}' not implemented"
                )
        return cls.from_list(vals, dtype)

    @classmethod
    def from_csv(
        cls, filename: str, field: str, dtype: dtypes.DataType | None = None, **kwargs
    ):
        # TODO: Implemented reading csv directly instead of using pandas
        df = pd.read_csv(filename, **kwargs)
        target = df[field].to_numpy()
        return cls.from_numpy(target, dtype)

    @singledispatchmethod
    @classmethod
    def new(
        cls,
        target,
        dtype: dtypes.DataType | None = None,
    ):
        """Create a new vector"""
        try:
            LOGGER.warn("Trying to iterate invalid input type")
            return cls.from_iter(iter(target), dtype)
        except TypeError:
            LOGGER.error("Trying to create Vector from invalid type")
            raise TypeError(
                f"Could not create `Vector` from type '{
                    type(target).__name__}'"
            )

    @new.register(list)
    @classmethod
    def _(
        cls,
        target: list,
        dtype: dtypes.DataType | None = None,
    ):
        return cls.from_list(target, dtype)

    @new.register(np.ndarray)
    @classmethod
    def _(
        cls,
        target: np.ndarray,
        dtype: dtypes.DataType | None = None,
    ):
        return cls.from_numpy(target, dtype)

    @new.register(Iterator)
    @classmethod
    def _(
        cls,
        target: Iterator,
        dtype: dtypes.DataType | None = None,
    ):
        return cls.from_iter(target, dtype)

    @new.register(Iterable)
    @classmethod
    def _(
        cls,
        target: Iterable,
        dtype: dtypes.DataType | None = None,
    ):
        return cls.from_iter(iter(target), dtype)

    @new.register(Real)
    @classmethod
    def _(
        cls,
        target: Real,
        dtype: dtypes.DataType | None = None,
    ):
        return cls.from_list([target], dtype)

    @property
    def len(self) -> int:
        """Length of the vector"""
        return self._cdata.len

    @property
    def capacity(self) -> int:
        """Allocated capacity of the vector"""
        return self._cdata.capacity

    @property
    def data_size(self) -> int:
        """Size in bytes of each element in the vector"""
        return self._cdata.data_size

    def __del__(self):
        try:
            ffi_call(
                _lionl.lion_vector_cleanup(self._app, self._cdata),
                "Failed cleanup of vector",
            )
        except LionException as e:
            LOGGER.error(f"Cleaning up vector failed with exception '{e}'")

    def __str__(self) -> str:
        return self.content_string()

    def __getitem__(self, key: int):
        return self.get(key)

    def __setitem__(self, key: int, value):
        return self.set_key(key, value)

    def __iter__(self) -> Self:
        self._index = 0
        return self

    def __next__(self):
        try:
            val = self[self._index]
        except IndexError:
            raise StopIteration
        self._index += 1
        return val

    def __len__(self) -> int:
        return self.len

    def __array__(self) -> np.ndarray:
        # TODO: Determine a more efficient way of converting to numpy array
        return np.array(self.to_list(), dtype=self._dtype.np)

    def string(self) -> str:
        """Turn vector into a string"""
        return self.__str__()

    def _generic_str(self) -> str:
        return f"Vector<{self._dtype.name}>"

    def content_string(self) -> str:
        """Get string with the content of this Vector"""
        return f"{self._generic_str()}{self.to_list()}"

    def short_string(self) -> str:
        """Get short version of a string of this Vector"""
        return f"{self._generic_str()}[{self.len}/{self.capacity}]"

    def validate_index(self, idx: int) -> int:
        """Validate whether a given index is in bounds"""
        if idx >= self.len:
            raise IndexError(f"index {idx} out of range for vector of len {self.len}")
        if idx < -self.len:
            raise IndexError(
                f"index {idx % self.len} out of range for vector of len {self.len}"
            )
        return idx % self.len

    def get(self, key: int):
        """Get an element at given index"""
        key = self.validate_index(key)
        val = ffi.new(f"{self._dtype.long_name} *")
        ffi_call(
            _lionl.lion_vector_get(self._app, self._cdata, key, val),
            f"Failed getting element at index {key}",
        )
        return val[0]

    def set_key(self, key: int, value) -> None:
        """Set element at given index"""
        key = self.validate_index(key)
        val = ffi.new(f"{self._dtype.long_name} *")
        val[0] = value
        ffi_call(
            _lionl.lion_vector_set(self._app, self._cdata, key, val),
            f"Failed setting element at key {key}",
        )

    def to_list(self) -> List:
        """Turn vector to a list"""
        return list(self)

    def to_numpy(self) -> np.ndarray:
        """Turn vector to a numpy array"""
        return np.array(self)

    def resize(self, new_capacity: int) -> None:
        """Resize this vector"""
        ffi_call(
            _lionl.lion_vector_resize(self._app, self._cdata, new_capacity),
            "Failed resizing",
        )

    def push(self, element) -> None:
        """Push an element into the vector"""
        val = ffi.new(f"{self._dtype.long_name} *")
        val[0] = element
        ffi_call(
            _lionl.lion_vector_push(self._app, self._cdata, val),
            "Failed pushing element",
        )

    def extend_from_list(self, target: List):
        """Extend this vector by a given list"""
        size = len(target)
        val = ffi.new(f"{self._dtype.long_name}[]", target)
        ffi_call(
            _lionl.lion_vector_extend_array(self._app, self._cdata, val, size),
            "Failed extending from list",
        )

    def extend_from_numpy(self, target: np.ndarray):
        """Extend this vector by a given numpy array"""
        target = target.flatten()
        size = len(target)
        val = ffi.new(f"{self._dtype.long_name}[]", target)
        ffi_call(
            _lionl.lion_vector_extend_array(self._app, self._cdata, val, size),
            "Failed extending from numpy array",
        )

    def extend_from_iter(self, target: Iterable):
        """Extend this vector by a given iterator"""
        self.extend_from_list(list(iter(target)))

    @singledispatchmethod
    def extend(self, target):
        """Extend this vector"""
        try:
            LOGGER.warn("Trying to extend from unsupported input type, trying as iter")
            return self.extend_from_iter(iter(target))
        except TypeError:
            LOGGER.error("Trying to extend Vector from invalid type")
            raise TypeError(
                f"Could not extend `Vector` from type '{
                    type(target).__name__}'"
            )

    @extend.register(list)
    def _(self, target):
        return self.extend_from_list(target)

    @extend.register(np.ndarray)
    def _(self, target):
        return self.extend_from_numpy(target)

    @extend.register(Iterator)
    def _(self, target):
        return self.extend_from_iter(target)

    @extend.register(Iterable)
    def _(self, target):
        return self.extend_from_iter(iter(target))

    def create_from(self) -> Self:
        """Create a new vector from this vector"""
        return Vector.with_capacity(self.capacity, self._dtype)
