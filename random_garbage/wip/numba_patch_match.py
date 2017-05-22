import numba
from numba import jit, vectorize, guvectorize, float32, int32
import numpy as np
import time
import matplotlib.pyplot as plt
import threading
import scipy.misc
import scipy.spatial

def make_window_indices(image):
    
    height, width, depth = image.shape

    indices = np.arange(height*width).reshape((height, width))

    i = np.arange(height)
    j = np.arange(width)
    j, i = np.meshgrid(j, i)

    window_indices = np.stack([
        flatten(indices[
            np.clip(i + di, 0, height - 1),
            np.clip(j + dj, 0, width  - 1)])
        
        for di in [-1, 0, 1]
        for dj in [-1, 0, 1]
    ], axis=-1)

    return window_indices

def flatten(x):
    return x.reshape(np.product(x.shape))

def knnsearch(dataset, testset, k):
    kdtree = scipy.spatial.cKDTree(dataset, leafsize=10)
    distances, indices = kdtree.query(testset, k, eps=2.0, n_jobs=4)
    return (distances, indices)

@jit(nopython=True, nogil=True)
def calculate_cost(A, B, iA, jA, iB, jB):
    A_height, A_width, A_depth = A.shape
    B_height, B_width, B_depth = B.shape
    
    r = np.arange(-1, 2)
    
    cost = 0.0
    for di in r:
        for dj in r:
            i0 = clamp(iA + di, 0, A_height - 1)
            j0 = clamp(jA + dj, 0, A_width  - 1)
            
            i1 = clamp(iB + di, 0, B_height - 1)
            j1 = clamp(jB + dj, 0, B_width  - 1)
            
            for k in range(3):
                delta = A[i0, j0, k] - B[i1, j1, k]
                cost += delta*delta
    
    return np.sqrt(cost)

@jit(nopython=True, nogil=True)
def improve(costs, iAB, jAB, A, B, iA, jA, iB, jB):

    cost = calculate_cost(A, B, iA, jA, iB, jB)

    if cost < costs[iA, jA]:
        costs[iA, jA] = cost
        iAB[iA, jA] = iB
        jAB[iA, jA] = jB

@jit(nopython=True, nogil=True)
def clamp(x, a, b):
    if x < a: return a
    if x > b: return b
    return x

@jit(nopython=True, nogil=True)
def minimize_cost(A, B):
    A_height, A_width, A_depth = A.shape
    B_height, B_width, B_depth = B.shape

    # indices from A into closest patch in B
    iAB = np.random.randint(0, B_height, (A_height, A_width))
    jAB = np.random.randint(0, B_width , (A_height, A_width))

    costs = 1e10*np.ones((A_height, A_width), dtype=np.float32)

    for iteration in range(5):
        print(iteration)
        
        iBs = np.random.randint(0, B_height, (A_height, A_width))
        jBs = np.random.randint(0, B_width , (A_height, A_width))

        for iA in range(A_height):

            for jA in range(A_width):
                iB = iBs[iA, jA]
                jB = jBs[iA, jA]

                improve(costs, iAB, jAB, A, B, iA, jA, iB, jB)

        r = np.arange(-1, 2)
        
        for iA in range(A_height):
            for jA in range(A_width):
                for di in r:
                    for dj in r:
                        iNeighbor = clamp(iA + di, 0, A_height - 1)
                        jNeighbor = clamp(jA + dj, 0, A_width  - 1)

                        iB = clamp(iAB[iNeighbor, jNeighbor] - di, 0, B_height - 1)
                        jB = clamp(jAB[iNeighbor, jNeighbor] - dj, 0, B_width  - 1)

                        improve(costs, iAB, jAB, A, B, iA, jA, iB, jB)
        
    return costs

A = scipy.misc.imread("tower1.png")/256.0
B = scipy.misc.imread("tower2.png")/256.0

A_height, A_width, A_depth = A.shape
B_height, B_width, B_depth = B.shape
features_A = A.reshape((A_height*A_width, A_depth))[make_window_indices(A)]
features_B = B.reshape((B_height*B_width, B_depth))[make_window_indices(B)]

features_A = features_A.reshape([len(features_A), -1])
features_B = features_B.reshape([len(features_B), -1])

t = time.clock()
distances, indices = knnsearch(features_A, features_B, k=1)
dt = time.clock() - t
print("%f seconds"%dt)
print("max distances: %f"%np.max(distances))

plt.subplot(1, 2, 1)
plt.imshow(distances.reshape((B_height, B_width)), cmap='gray')
plt.axis('off')

print("\n")

t = time.clock()
costs = minimize_cost(B, A)
dt = time.clock() - t
print("%f seconds"%dt)
print("max costs: %f"%np.max(costs))

plt.subplot(1, 2, 2)
plt.imshow(costs, cmap='gray')
plt.axis('off')

plt.show()


"""
# jit decorator tells Numba to compile this function.
# The argument types will be inferred by Numba when function is called.
@jit(float32(float32[:, :]), nopython=True, nogil=True)
def sum2d(arr):
    M, N = arr.shape
    result = 0.0
    for i in range(M):
        for j in range(N):
            result += arr[i,j]
    return result

@vectorize([float32(float32, float32)])
def f(x, y):
    return x + y

@guvectorize([(float32[:], float32[:], float32[:])], '(n),()->(n)')
def g(x, y, res):
    for i in range(x.shape[0]):
        res[i] = x[i] + y[0]

n = 1000
a = np.ones((n, n), dtype=np.float32)

a = f(a, a)

for _ in range(10):
    t = time.clock()
    result = sum2d(a)
    dt = time.clock() - t
    print(result, dt)
"""
"""
@jit(nopython=True, nogil=True)
def mandel(x, y, max_iters):

    Given the real and imaginary parts of a complex number,
    determine if it is a candidate for membership in the Mandelbrot
    set given a fixed number of iterations.

    c = complex(x,y)
    z = 0j
    for i in range(max_iters):
        z = z*z + c
        if z.real * z.real + z.imag * z.imag >= 4:
            return 255 * i // max_iters

    return 255

@jit(nopython=True, nogil=True)
def create_fractal(min_x, max_x, min_y, max_y, image, iters):
    height = image.shape[0]
    width = image.shape[1]

    pixel_size_x = (max_x - min_x) / width
    pixel_size_y = (max_y - min_y) / height
    for x in range(width):
        real = min_x + x * pixel_size_x
        for y in range(height):
            imag = min_y + y * pixel_size_y
            color = mandel(real, imag, iters)
            image[y, x] = color

    return image

for _ in range(10):
    t = time.clock()

    image = np.zeros((700, 1400), dtype=np.uint8)

    create_fractal(-2.0, 1.0, -1.0, 1.0, image, 20)
    
    dt = time.clock() - t
    print(dt)
    
for _ in range(10):
    t = time.clock()

    image = np.zeros((700, 1400), dtype=np.uint8)

    threads = [
        threading.Thread(target=create_fractal, args=[-2.0, -0.5, -1.0, 0.0, image[:350, :700], 20]),
        threading.Thread(target=create_fractal, args=[-0.5,  1.0, -1.0, 0.0, image[:350, 700:], 20]),
        threading.Thread(target=create_fractal, args=[-0.5,  1.0,  0.0, 1.0, image[350:, 700:], 20]),
        threading.Thread(target=create_fractal, args=[-2.0, -0.5,  0.0, 1.0, image[350:, :700], 20]),
    ]

    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    
    dt = time.clock() - t
    print(dt)

for value in create_fractal.inspect_asm().values():
    with open("tmp.txt", "w") as f:
        f.write(value)

plt.imshow(image, cmap='gray')
plt.show()
"""
