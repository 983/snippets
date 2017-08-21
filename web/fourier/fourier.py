import numpy as np
import matplotlib.pyplot as plt
import time, sys
from numba import njit

@njit
def bitreverse(x, n):
    result = 0
    for i in range(n):
        bit = (x >> i) & 1
        result = (result << 1) | bit
    return result

@njit
def bitlength(x):
    n = 0
    while x > 0:
        x >>= 1
        n += 1
    return n

def make_W(n):
    W = np.zeros((n, n), dtype=np.complex128)
    for i in range(n):
        for j in range(n):
            W[i, j] = np.exp(-2.0*1j*np.pi*i*j/n)
    return W

def fft(c):
    n = len(c)
    
    if n == 1: return c

    #if n <= threshold:
    #    return np.dot(W, c)
    
    a = fft(c[0::2])
    b = fft(c[1::2])

    k = np.arange(n//2)
    w = np.exp(-2.0*1j*np.pi*k/n)

    p = a + w*b 
    q = a - w*b
    
    c = np.concatenate([p, q])
    
    return c

@njit
def swap(a, i, j):
    temp = a[i]
    a[i] = a[j]
    a[j] = temp

@njit
def fft2(re, im):
    n = len(re)
    log2_n = bitlength(n) - 1

    angles = -2.0*np.pi/n*np.arange(n)
    c = np.cos(angles)
    s = np.sin(angles)

    for i in range(n):
        j = bitreverse(i, log2_n)
        if i < j:
            swap(re, i, j)
            swap(im, i, j)

    for shift in range(1, log2_n + 1):
        block_size = 1 << shift
        cos_sin_stride = n // block_size

        for block_offset in range(0, n, block_size):
            
            half_block_size = block_size//2
            
            for i in range(half_block_size):
                ar = re[block_offset + i];
                ai = im[block_offset + i];
                br = re[block_offset + i + half_block_size];
                bi = im[block_offset + i + half_block_size];
                cr = c[i*cos_sin_stride];
                ci = s[i*cos_sin_stride];
                dr = br*cr - bi*ci;
                di = br*ci + bi*cr;
                
                re[block_offset + i                  ] = ar + dr;
                im[block_offset + i                  ] = ai + di;
                re[block_offset + i + half_block_size] = ar - dr;
                im[block_offset + i + half_block_size] = ai - di;

"""
np.random.seed(0)
n = 4
a = np.random.rand(n) + 1j*np.random.rand(n)
b = np.fft.fft(a)
c = fft2(a.real, a.imag)
d = b - c
print(np.max(np.abs(d)))
"""

for i in range(10):
    threshold = 1 << i
    W = make_W(threshold)

    print(threshold)
    n = 1024
    for _ in range(10):
        a = np.random.rand(n) + 1j*np.random.rand(n)
        t0 = time.clock()
        b = np.fft.fft(a)
        t1 = time.clock()
        fft2(a.real, a.imag)
        t2 = time.clock()
        d = np.abs(b - a)
        print((t1 - t0)/(t2 - t1), np.max(d))
    print("")

"""
n = 1024
t = np.linspace(0, 1, n)

x = np.cos(t*2*np.pi)
y = np.sin(t*2*np.pi)

data = x + y*1j

a = np.fft.fft(data)
#b = a.copy()
#b[205:] = 0
c = np.fft.ifft(a)

plt.plot(x, y)
plt.plot(c.real, c.imag)
plt.show()
"""
