import numpy as np

def lup(A):
    A = A.copy()
    n = len(A)
    P = np.arange(n)

    for i in range(n):

        # find element with largest absolute value in column i starting at i
        imax = i + np.argmax(np.abs(A[i:, i]))

        if A[imax, i] == 0.0:
            raise ValueError("Matrix is degenerate")

        # pivoting by swapping rows of A
        if i != imax:
            A[[i, imax]] = A[[imax, i]]
            P[[i, imax]] = P[[imax, i]]

        for j in range(i + 1, n):
            A[j, i] /= A[i, i]

            for k in range(i + 1, n):
                A[j, k] -= A[j, i]*A[i, k]

    return A, P

def lup_solve(decomposition, b):
    A, P = decomposition
    n = len(A)
    x = np.zeros_like(b)

    for i in range(n):
        x[i] = b[P[i]]

        for k in range(i):
            x[i] -= A[i, k]*x[k]

    for i in reversed(range(n)):
        for k in range(i + 1, n):
            x[i] -= A[i, k]*x[k]

        x[i] /= A[i, i]

    return x

def lup_inv(decomposition):
    A, P = decomposition
    n = len(A)
    IA = np.zeros((n, n))
    
    for j in range(n):
        for i in range(n):
            IA[i, j] = float(P[i] == j)

            for k in range(i):
                IA[i, j] -= A[i, k]*IA[k, j]

        for i in reversed(range(n)):
            for k in range(i + 1, n):
                IA[i, j] -= A[i, k]*IA[k, j]

            IA[i, j] /= A[i, i]
    
    return IA

n_ok = 0
n_singular = 0

for _ in range(1000):
    n = np.random.randint(1, 10)
    A = np.random.randn(n, n)

    A[np.random.randn(n, n) < 0.3] = 0

    vals = np.linalg.eigvals(A)
    if np.min(np.abs(vals)) < 1e-8:
        n_singular += 1
        continue

    b = np.random.randn(n)

    x1 = np.linalg.solve(A, b)

    decomposition = lup(A)
    x2 = lup_solve(decomposition, b)
    IA = lup_inv(decomposition)

    assert(np.allclose(x1, x2))
    assert(np.allclose(A@IA, np.eye(n)))
    
    n_ok += 1

print("%d ok"%n_ok)
print("%d singular"%n_singular)
