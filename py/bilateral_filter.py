import numpy as np
import matplotlib.pyplot as plt
import scipy.misc
from numba import jit
import os, sys, time

n = 64
x = np.linspace(-1, 1, n)
y = np.linspace(-1, 1, n)
x, y = np.meshgrid(x, y)

def smoothstep(x, a, b):
    t = np.clip((x - a)/(b - a), 0, 1)
    return t*t*(3 - 2*t)

image = smoothstep(np.sqrt(x*x + y*y), 0.4, 0.6)
image += np.random.rand(*image.shape)*0.1 - 0.05

path = "test.png"
#image = scipy.misc.imread(path, flatten=True)/256.0

sigma_s = 8.0
sigma_r = 0.2

r = 3
x = np.linspace(-r, r, 2*r + 1)
y = np.linspace(-r, r, 2*r + 1)
x, y = np.meshgrid(x, y)
domain_filter = np.exp(-(x*x + y*y)/(2*sigma_s*sigma_s))

padded_image = np.pad(image, ((r, r), (r, r)), mode='edge')

@jit(nopython=True)
def bilateral_filter(image):

    height, width = image.shape

    result = np.zeros_like(image)

    for i in range(height):
        for j in range(width):
            I = padded_image[i:i+2*r+1, j:j+2*r+1]
            p = image[i, j]

            range_filter = np.exp(-(I - p)**2/(2*sigma_r*sigma_r))

            t = range_filter * domain_filter
            
            result[i, j] = np.sum(t*I)/np.sum(t)
    return result

result = image
for iteration in range(10):
    print("iteration %d"%iteration)
    result = bilateral_filter(result)
    
plt.subplot(1, 2, 1)
plt.imshow(image, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(1, 2, 2)
plt.imshow(result, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.show()

