from lion import Vector, dtypes


def test_zero_i32():
    CAP = 10
    a = Vector.zero(CAP, dtypes.INT32)
    assert a.len == CAP
    assert a.capacity == CAP
    for i in range(CAP):
        assert a[i] == 0
    assert a.to_list() == [0 for _ in range(10)]
    a[1] = 15
    new_list = [0 for _ in range(10)]
    new_list[1] = 15
    assert a.to_list() == new_list

    new_list = [*new_list, 1, 3, 8]
    a.push(1)
    a.push(3)
    a.push(8)
    assert a.to_list() == new_list
    assert a.len == 13
    assert a.capacity == 20

    new_list = [*new_list, 6, 1, 8, 2]
    a.extend_from_list([6, 1, 8, 2])
    assert a.to_list() == new_list


def test_list_i32():
    b = Vector.from_list([1, 25, 2, 111, -1])
    assert b.to_list() == [1, 25, 2, 111, -1]
    assert b.len == 5
    assert b.capacity == 5


def test_zero_f64():
    CAP = 10
    a = Vector.zero(CAP, dtypes.FLOAT64)
    assert a.len == CAP
    assert a.capacity == CAP
    for i in range(CAP):
        assert a[i] == 0.0
    assert a.to_list() == [0.0 for _ in range(10)]
    a[1] = 15.5
    new_list = [0.0 for _ in range(10)]
    new_list[1] = 15.5
    assert a.to_list() == new_list

    new_list = [*new_list, 1.0, 3.0, 8.0]
    a.push(1)
    a.push(3)
    a.push(8)
    assert a.to_list() == new_list
    assert a.len == 13
    assert a.capacity == 20

    new_list = [*new_list, 6.0, 1.0, 8.0, 2.0]
    a.extend_from_list([6, 1, 8, 2])
    assert a.to_list() == new_list


def test_list_f64():
    b = Vector.from_list([1.15, 25.2, 2.7, 111.1245125, 0.0, -1])
    assert b.to_list() == [1.15, 25.2, 2.7, 111.1245125, 0.0, -1.0]
    assert b.len == 6
    assert b.capacity == 6
