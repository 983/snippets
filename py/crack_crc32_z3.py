#!/usr/bin/python
from z3 import *
import zlib
import time

def crc32(data, crc=0, polynomial=0xedb88320):
    crc ^= 0xffffffff
    for x in data:
        crc ^= ZeroExt(24, x)
        for _ in range(8):
            is_odd = (crc & 1) == 1
            shifted = LShR(crc, 1)
            crc = If(is_odd, shifted ^ polynomial, shifted)
    return crc ^ 0xffffffff

target_crc = 0
number_of_bytes = 10
prefix = b"This is a prefix"
suffix = b"This is a really very extremely long suffix"

data = [BitVec(f"byte_{i}", 8) for i in range(number_of_bytes)]
prefix_data = [BitVec(f"prefix_{i}", 8) for i in range(len(prefix))]
suffix_data = [BitVec(f"suffix_{i}", 8) for i in range(len(suffix))]

solver = Solver()

solver.add(crc32(suffix_data, crc32(data, crc32(prefix_data))) == target_crc)

for x, v in zip(prefix, prefix_data):
    solver.add(x == v)
for x, v in zip(suffix, suffix_data):
    solver.add(x == v)
for x in data:
    solver.add(And(ord('0') <= x, x <= ord('9')))

t = time.perf_counter()

if solver.check() == sat:

    m = solver.model()

    b = prefix + bytes(m[x].as_long() for x in data) + suffix

    print("Checking CRC32 of", b.decode("utf-8"))
    print("want:", target_crc)
    print("have:", zlib.crc32(b))
    print("success :)")
else:
    print("Could not solve :(")
    print("Constraints are too strict for given number of bits")
