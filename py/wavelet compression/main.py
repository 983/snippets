import numpy as np
import scipy.optimize
import scipy.misc
import scipy.signal
import scipy.ndimage
import matplotlib.pyplot as plt
import pywt
import random, time, sys, os
from flatten_nested import flatten, unflatten, get_structure
from find_scale_offset import find_scale_offset
import bz2
import lz4.frame

h = np.array([0.48296291, 0.8365163, 0.22414387, -0.12940952])
g = np.array([h[3], -h[2], h[1], -h[0]])

def make_db2(n):
    T = np.zeros((n, n))
    
    T = np.zeros((n, n))
    for i in range(n//2):
        for j in range(4):
            T[i +    0, (i*2 + j) % n] += h[j]
            T[i + n//2, (i*2 + j) % n] += g[j]

    return T

def transform_rows(rows, inverse=False):
    n = len(rows[0])
    T = make_db2(n)

    if inverse:
        T = T.T

    return np.array([T@row for row in rows])

path = "einstein.jpg"

image0 = scipy.misc.imread(path, flatten=True)
#image0 = scipy.misc.imresize(image0, (64, 64))
image0 = image0/256.0
image0 -= np.mean(image0)
image = image0.copy()

def transform2d(image, inverse=False):
    ns = [image.shape[0]]
    while ns[-1]//2 >= 4:
        ns.append(ns[-1]//2)

    if inverse:
        ns = reversed(ns)
    
    for n in ns:
        subimage = image[:n, :n]

        # note that it does not matter whether the rows or cols are transformed first
        
        subimage = transform_rows(subimage, inverse=inverse)
        subimage = subimage.T
        subimage = transform_rows(subimage, inverse=inverse)
        subimage = subimage.T
        
        image[:n, :n] = subimage

        #plt.imshow(image, cmap='gray');plt.show()

method = "pywt"
method = "dct"
method = "T db2"

print("method:", method)

if method == "pywt":
    wavelet = 'sym9'
    wavelet = 'haar'
    wavelet = 'db4'
    wavelet = 'db2'
    wavelet = pywt.Wavelet(wavelet)

    image = pywt.wavedec2(image, wavelet, mode='periodization')

if method == "T db2":
    transform2d(image)

if method == 'dct':
    image = scipy.fftpack.dct(image, axis=0)
    image = scipy.fftpack.dct(image, axis=1)

flat = flatten(image)

print("bounds:", np.min(flat), np.max(flat))

if 1:
    # threshold
    a = np.abs(flat)
    threshold = np.sort(a)[int(len(a)*0.95)]
    mask = a >= threshold
    flat[~mask] = 0

if 1:
    # quantize
    c = flat[mask]
    signs = c < 0
    c = np.abs(c)
    c = np.log(c)
    c0 = np.min(c)
    c1 = np.max(c)
    u = (c - c0)/(c1 - c0)
    scale = 1024
    u = np.floor(u*scale)

    data = np.concatenate([u, signs, mask]).astype(np.uint8).tobytes()
    compressed = bz2.compress(data)
    #compressed = lz4.frame.compress(data)

    print("compression factor:", np.product(image.shape)/len(compressed))
    print("compressed to %d bytes"%len(compressed))
    print("without transform:", len(bz2.compress((image0*255).astype(np.uint8).tobytes())))
    
    u /= scale
    c = u*(c1 - c0) + c0
    c = np.exp(c)
    c[signs] *= -1
    flat[mask] = c

image = unflatten(flat, image)

if method == "dct":
    image = scipy.fftpack.idct(image, axis=0)
    image = scipy.fftpack.idct(image, axis=1)

    image *= np.max(image0)/np.max(image)

if method == "pywt":
    image = pywt.waverec2(image, wavelet, mode='periodization')

if method == "T db2":
    transform2d(image, inverse=True)

print("error:", np.linalg.norm(image - image0))

plt.imshow(image, cmap='gray')
plt.show()
