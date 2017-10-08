import scipy.misc
import scipy.optimize
import scipy.signal
import scipy.stats
import matplotlib.pyplot as plt
import numpy as np
#import autograd.numpy as np
#from autograd import grad
import random, time, sys, os

def close(points):
    return np.concatenate([points[-1:], points])

def calculate_cost(indices):
    x, y = close(points[indices]).T
    dx = x[1:] - x[:-1]
    dy = y[1:] - y[:-1]
    return np.sum(np.sqrt(dx*dx + dy*dy))

def swap_pair(indices):
    indices = indices.copy()
    i, j = np.random.randint(n, size=2)
    indices[[i, j]] = indices[[j, i]]
    return indices

def reverse_subpath(indices):
    i, j = np.sort(np.random.randint(n, size=2))

    a = indices[:i]
    bc = indices[i:j]
    d = indices[j:]

    return np.concatenate([a, bc[::-1], d])

def improve(indices, method):
    new_indices = method(indices)

    new_cost = calculate_cost(new_indices)

    if calculate_cost(indices) > new_cost:
        return new_indices

    return indices

def make_random_solution():
    indices = np.arange(n)
    np.random.shuffle(indices)
    return indices

def cross(a, b):
    i, j = np.random.randint(n, size=2)
    
    c = a[i:j]

    # d = b \ c where b is shifted by j
    keep = np.ones(n, dtype=np.bool)
    keep[c] = 0
    b2 = np.concatenate([b[j:], b[:j]])
    d = b2[keep[b2]]
    
    return np.concatenate([c, d])

np.random.seed(0)
random.seed(0)

# n = 1000: 100000 35.689320
# n = 100: 010000 8.315573
n = 100
points = np.random.rand(n, 2)

solutions = [make_random_solution() for _ in range(100)]

for iteration in range(10*1000 + 1):
    solutions = list(sorted(solutions, key=calculate_cost))
    
    if iteration % 100 == 0:
        print("%06d %f"%(iteration, calculate_cost(solutions[0])))

    new_solutions = []
    for solution in solutions:
        
        if np.random.rand() < 0.5:
            n_best = 10
            other = random.choice(solutions[:n_best])
            solution = cross(solution, other)

        if np.random.rand() < 0.5:
            solution = reverse_subpath(solution)
        
        new_solutions.append(solution)

    solutions = new_solutions

if 1:
    solutions = list(sorted(solutions, key=calculate_cost))

    indices = solutions[0]

    x, y = close(points[indices]).T

    plt.plot(x, y, 'ro')
    plt.plot(x, y, color='blue')
    plt.axes().set_aspect('equal', 'datalim')
    plt.show()

if 1:
    solutions = list(sorted(solutions, key=calculate_cost))
    
    m = len(solutions)
    image = np.zeros((m, m))
    for i in range(m):
        for j in range(m):
            a = calculate_cost(solutions[i])
            b = calculate_cost(solutions[j])
            image[i, j] = np.abs(a - b)

    plt.imshow(image, cmap='gray', interpolation=None)
    plt.show()
