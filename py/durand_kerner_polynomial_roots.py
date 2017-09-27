import numpy as np
import matplotlib.pyplot as plt

np.random.seed(0)

n_coeffs = 20
coeffs = np.random.randn(n_coeffs)

def poly_at(coeffs, x):
    y = 0

    for coeff in coeffs:
        y = x*y + coeff

    return y

n_roots = n_coeffs - 1
roots = [1]
for i in range(1, n_roots):
    roots.append(roots[-1] * (0.4 + 0.8j))
roots = np.array(roots)

for iteration in range(1000):
    max_change = 0

    new_roots = np.zeros_like(roots)

    max_change = 0

    for i in range(n_roots):
        root = roots[i]

        change = poly_at(coeffs, root)

        p = 1

        for j in range(n_roots):
            if i != j:
                p = p*(root - roots[j])

        p = coeffs[0]*p

        change = change/p

        new_roots[i] = root - change

        max_change = max(max_change, np.abs(change))

    roots = new_roots

    if max_change < 1e-5:
        break

print(iteration, "iterations")

p = np.poly1d(coeffs)
roots2 = p.roots

print(p(roots))
print(p(roots2))
