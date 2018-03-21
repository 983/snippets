import numpy as np
import random
import math
import time
import sys

def make_W(n, sign=1):
    W = [[0]*n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            angle = sign*-2*math.pi*i*j/n
            W[i][j] = np.cos(angle) + 1j*np.sin(angle)
    return W

def dot(a, b):
    return sum(ai*bi for ai,bi in zip(a, b))

def mul_mat_vec(A, v):
    return [dot(row, v) for row in A]

def dft(values):
    return np.fft.rfft(values)

    W = make_W(len(values))
    return mul_mat_vec(W, values)

def idft(values):
    return np.fft.irfft(values)

    n = len(values)
    W = make_W(n, -1)
    result = mul_mat_vec(W, values)
    # normalize
    return [x/n for x in result]

np.random.seed(0)

if 0:
    a = np.random.rand(123)
    b0 = dct(a)
    b1 = np.fft.fft(a)
    print(np.max(np.abs(b0 - b1)))
    c0 = idct(b0)
    c1 = np.fft.ifft(b1)
    print(np.max(np.abs(c0 - c1)))

def pad(values, n):
    padded_values = np.zeros(n, dtype=values.dtype)
    padded_values[:len(values)] = values
    return padded_values
    
    result = [0]*n
    for i, value in enumerate(values):
        result[i] = value
    return result

base_bit_length = 8
base = 1 << base_bit_length

def bytes_to_int(values):
    result = 0
    for i, x in enumerate(values):
        result |= x << i*base_bit_length
    return result

def int_to_bytes(value):
    values = []
    while value > 0:
        values.append(value & (base - 1))
        value >>= base_bit_length
    return values

def mul(a, b):
    a = int_to_bytes(a)
    b = int_to_bytes(b)

    na = len(a)
    nb = len(b)
    
    c = [0]*(na + nb)
    
    for i, ai in enumerate(a):
        for j, bj in enumerate(b):
            c[i + j] += ai*bj

    c = [int(x) for x in c]

    propagate_carry(c)

    c = bytes_to_int(c)
    
    return c

def next_power_of_two(x):
    return 1 << (x-1).bit_length()

def div_round_up(a, b):
    return (a + b - 1)//b

from numba import njit
@njit(cache=True)
def propagate_carry(c):
    carry = 0
    for i in range(len(c)):
        temp = c[i] + carry
        c[i] = temp & 255
        carry = temp >> 8

deltas = np.zeros(20)

def fftmul(a, b):
    t = [0]*(len(deltas) + 1)

    t[0] = time.clock()
    
    na = div_round_up(a.bit_length(), 8)
    nb = div_round_up(b.bit_length(), 8)

    t[1] = time.clock()
    
    a = a.to_bytes(na, byteorder='little')
    b = b.to_bytes(nb, byteorder='little')

    t[2] = time.clock()
    
    pad_length = next_power_of_two(max(na, nb))*2

    t[3] = time.clock()

    a = np.frombuffer(a, dtype=np.uint8)
    b = np.frombuffer(b, dtype=np.uint8)

    t[4] = time.clock()
    
    a = pad(a, pad_length)
    b = pad(b, pad_length)

    t[5] = time.clock()

    c = idft(dft(a)*dft(b))

    t[6] = time.clock()

    c = c.real.round().astype(np.uint64)
    
    #c = [int(round(x.real)) for x in c]

    t[7] = time.clock()

    propagate_carry(c)

    t[8] = time.clock()

    c = c.astype(np.uint8)

    t[9] = time.clock()

    c = c.tobytes()

    t[10] = time.clock()

    c = int.from_bytes(c, byteorder='little')
    
    t[11] = time.clock()

    for i in range(20):
        deltas[i] += t[i + 1] - t[i]

    return c

for i in range(1*1000*1000, 10*1000*1000, 1000*1000):
#for i in range(1, 1000, 100):
    if random.random() < 0.5:
        x = random.randint(1, 1 << i)
        y = random.randint(1, 1 << i)
    else:
        x = (1 << i) - 1
        y = (1 << i) - 1

    t0 = time.clock()
    z0 = x*y
    t1 = time.clock()
    #z1 = mul(x, y)
    z2 = fftmul(x, y)
    t2 = time.clock()

    if z0 != z2:
    #if z0 != z1 or z1 != z2:
        print("Error at bit length", i)
        #print(hex(z0))
        #print(hex(z1))
        #print(hex(z2))
        sys.exit(0)

    if i % 100 == 0:
        print("%d ok"%i)
    print(t1 - t0)
    print(t2 - t1)
    print("")

    assert(z0 == z2)

for i, delta in enumerate(deltas):
    print(i, delta)
