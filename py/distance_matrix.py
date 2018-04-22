import scipy.spatial
import numpy as np
import time
from numba import njit

def distance_matrix_slow(a, b):
    assert(a.shape[1] == b.shape[1])

    na = len(a)
    nb = len(b)
    n = a.shape[1]

    temp = np.zeros((na, nb, n))
    
    for i in range(na):
        for j in range(nb):
            for k in range(n):
                temp[i, j, k] = a[i, k] - b[j, k]

    temp *= temp
    
    return np.sqrt(np.sum(temp, axis=2))

def distance_matrix_slow2(a, b):
    assert(a.shape[1] == b.shape[1])

    na = len(a)
    nb = len(b)
    n = a.shape[1]

    distances = np.zeros((na, nb))
    
    for i in range(na):
        for j in range(nb):
            distances[i, j] = np.linalg.norm(a[i] - b[j])

    return distances

def distance_matrix(a, b):
    assert(a.shape[1] == b.shape[1])

    na = len(a)
    nb = len(b)
    n = a.shape[1]

    temp = a[:, np.newaxis, :] - b[np.newaxis, :, :]

    temp *= temp
    
    return np.sqrt(np.sum(temp, axis=2))

@njit
def distance_matrix_numba(a, b):
    assert(a.shape[1] == b.shape[1])

    na = len(a)
    nb = len(b)
    
    n = a.shape[1]

    distances = np.zeros((na, nb))
    
    for i in range(na):
        for j in range(nb):
            dist2 = 0.0
            for k in range(n):
                difference = a[i, k] - b[j, k]
                dist2 += difference*difference
            distances[i, j] = np.sqrt(dist2)

    return distances

np.random.seed(0)

for _ in range(10):
    a = np.random.rand(1000, 5)
    b = np.random.rand(100, 5)

    functions = [
        scipy.spatial.distance_matrix,
        scipy.spatial.distance.cdist,
        distance_matrix_slow,
        distance_matrix_slow2,
        distance_matrix,
        distance_matrix_numba,
    ]

    prev_result = None

    for func in functions:
        t = time.perf_counter()
        
        result = func(a, b)

        dt = time.perf_counter() - t

        if prev_result is not None:
            assert(np.allclose(result, prev_result))

        prev_result = result
        
        print("%21s: %f seconds"%(func.__name__, dt))

    print("")
