import numpy as np
from numba import njit
import matplotlib.pyplot as plt
import time, sys
import itertools

np.random.seed(0)

n = 64

x = np.linspace(-1, 1, n)
y = np.linspace(-1, 1, n)
x, y = np.meshgrid(x, y)
r = np.sqrt(x*x + y*y)
mask = r < 0.5
mask = np.logical_or(mask, np.random.rand(n, n) < 0.01)

def make_neighbors(r):
    neighbors = [(dx, dy) for dy in range(-r, r + 1) for dx in range(-r, r + 1)]
    return np.array(neighbors)

@njit
def calculate_df(mask, neighbors):
    height, width = mask.shape
    
    df = np.zeros((height, width))

    for y in range(height):
        for x in range(width):
            df[y, x] = np.inf
            for dx in range(-1, 2):
                for dy in range(-1, 2):
                    x2 = x + dx
                    y2 = y + dy
                    if x2 >= 0 and x2 < width and y2 >= 0 and y2 < height:
                        if mask[y, x] != mask[y2, x2]:
                            df[y, x] = min(df[y, x], 0.5*np.sqrt(dx*dx + dy*dy))
    
    distances = np.sqrt(neighbors[:, 0]**2 + neighbors[:, 1]**2)
    
    for y in range(height):
        for x in range(width):
            for i in range(len(neighbors)):
                x2 = x + neighbors[i, 0]
                y2 = y + neighbors[i, 1]
                if x2 >= 0 and x2 < width and y2 >= 0 and y2 < height:
                    df[y, x] = min(df[y, x], df[y2, x2] + distances[i])

    for y in range(height - 1, -1, -1):
        for x in range(width - 1, -1, -1):
            for i in range(len(neighbors)):
                x2 = x - neighbors[i, 0]
                y2 = y - neighbors[i, 1]
                if x2 >= 0 and x2 < width and y2 >= 0 and y2 < height:
                    df[y, x] = min(df[y, x], df[y2, x2] + distances[i])

    for y in range(height):
        for x in range(width):
            if mask[y, x]:
                df[y, x] = -df[y, x]
            
    return df

if 1:
    neighbors = make_neighbors(5)
    df2 = calculate_df(mask, neighbors)
    #np.save("df2.npy", df2)
#df2 = np.load("df2.npy")

neighbors = np.array([[-1, -1], [0, -1], [1,  -1], [-1, 0]])

df3 = calculate_df(mask, neighbors)

difference = np.abs(df2 - df3)

print("max difference:", np.max(difference))

if 1:
    plt.subplot(2, 2, 1)
    plt.imshow(mask, cmap='gray', interpolation='nearest')
    plt.axis('off')

    plt.subplot(2, 2, 2)
    plt.imshow(df2, cmap='gray', interpolation='nearest')
    plt.axis('off')

    plt.subplot(2, 2, 3)
    plt.imshow(df3, cmap='gray', interpolation='nearest')
    plt.axis('off')

    plt.subplot(2, 2, 4)
    plt.imshow(difference, cmap='gray', interpolation='nearest')
    plt.axis('off')

    plt.show()
    sys.exit(0)

# find best pattern
"""
results = []
best = (1e20, 20, None)

neighbors = make_neighbors(1)
neighbors = np.array(neighbors)
for mask1 in itertools.product([True, False], repeat=9):
    #print(mask1)
    for mask2 in itertools.product([True, False], repeat=9):

        neighbors1 = neighbors[np.array(mask1)]
        neighbors2 = neighbors[np.array(mask2)]

        if np.sum(mask1) == 0 or np.sum(mask2) == 0:
            continue
        
        t = time.clock()
        df3 = calculate_df(df, neighbors1, neighbors2)
        dt = time.clock() - t

        error = np.max(np.abs(df2 - df3))

        value = (error, np.sum(mask1) + np.sum(mask2), mask1, mask2, neighbors1, neighbors2)
        results.append(value)
        
        if value < best:
            best = value
            print(best)
"""
