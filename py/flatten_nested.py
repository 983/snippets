import numpy as np
from collections import Sequence

def _flatten(values):
    if isinstance(values, Sequence):
        for value in values:
            yield from _flatten(value)
    else:
        yield values

def flatten(values):
    # flatten nested lists of np.ndarray to np.ndarray
    return np.concatenate([value.flatten() for value in _flatten(values)])

def _unflatten(flat_values, structure, offset):
    if isinstance(structure, Sequence):
        result = []
        for value in structure:
            value, offset = _unflatten(flat_values, value, offset)
            result.append(value)
        return result, offset
    else:
        shape = structure.shape
        new_offset = offset + np.product(shape)
        value = flat_values[offset:new_offset].reshape(shape)
        return value, new_offset

def unflatten(flat_values, structure):
    # unflatten np.ndarray to nested lists with structure of prototype
    result, offset = _unflatten(flat_values, structure, 0)
    assert(offset == len(flat_values))
    return result

class Structure(object):
    def __init__(self, shape):
        self.shape = shape

def get_structure(values):
    if isinstance(values, Sequence):
        return [get_structure(value) for value in values]
    else:
        return Structure(values.shape)

if 0:
    # Example
    a = [
        np.random.rand(1),
        [
            np.random.rand(2, 1),
            np.random.rand(1, 2, 1),
        ],
        [[]],
    ]

    b = flatten(a)

    c = unflatten(b, a)

    structure = get_structure(a)

    d = unflatten(b, structure)

    print("a:")
    print(a)
    print("b:")
    print(b)
    print("c:")
    print(c)
    print("d:")
    print(d)
    print("")
