import numpy as np
import matplotlib.pyplot as plt
from numba import njit
import time

np.random.seed(1)

@njit("void(f8[:, :], i4[:, :])")
def all_pairs_shortest_path(d, next):
    # Floyd-Warshall algorithm
    n = d.shape[0]
    
    for k in range(n):
        for i in range(n):
            for j in range(n):
                detour = d[i, k] + d[k, j]
                
                if detour < d[i, j]:
                    d[i, j] = detour  
                    next[i, j] = next[i, k]

def main():
    w = 20
    h = 10
    
    mask = np.random.rand(h, w) < 0.1
    mask[0, 0] = 0
    
    n = w * h
    
    indices = np.arange(h * w).reshape(h, w)
    
    coordinates = np.empty((n, 2), dtype=np.int32)
    
    d = np.full((n, n), np.inf)
    
    next = np.full((n, n), -1, dtype=np.int32)
    
    eight_neighborhood = [
        (-1, -1), (0, -1), (1, -1),
        (-1,  0),          (1,  0),
        (-1,  1), (0,  1), (1,  1),
    ]
    
    for y in range(h):
        for x in range(w):
            i = indices[y, x]
            
            coordinates[i] = (x, y)
            
            d[i, i] = 0.0
            next[i, i] = i
            
            if mask[y, x]: continue
            
            for dy in range(-1, 2):
                for dx in range(-1, 2):
                    x2 = x + dx
                    y2 = y + dy
                    
                    if dx == 0 and dy == 0: continue
                    if x2 < 0 or x2 >= w or y2 < 0 or y2 >= h: continue
                    if mask[y2, x2]: continue
                    if abs(dx) + abs(dy) == 2 and (mask[y + dy, x] or mask[y, x + dx]): continue
                
                    j = indices[y2, x2]
                    
                    next[i, j] = j
                    
                    d[i, j] = np.sqrt(dx * dx + dy * dy)
    
    t = time.perf_counter()
    
    all_pairs_shortest_path(d, next)
    
    dt = time.perf_counter() - t
    
    print("%f seconds" % dt)
    
    plt.imshow(mask, interpolation='nearest')
    
    x = 0
    y = 0
    
    x_end = w - 1
    y_end = h - 1
    
    i = indices[y, x]
    j = indices[y_end, x_end]
    
    path = [i]
    
    if next[i, j] == -1:
        raise ValueError("Path does not exist")
    
    while i != j:
        i = next[i, j]
        path.append(i)
    
    path = np.array(path)
    
    points = coordinates[path]
    x, y = np.array(points).T
    plt.plot(x, y)
    
    plt.show()
    
    plt.imshow(d, cmap='gray')
    plt.show()

main()
