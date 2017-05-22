import numpy as np

np.random.seed(0)

n = 10
A = np.random.random((n, n))
A = 0.5*(A + A.T)
A += n*n*np.eye(n)

b = np.random.random((n, 1))

def f(x):
    return 0.5*x.T.dot(A).dot(x) - b.T.dot(x)

def df(x):
    return A.dot(x) - b

x1 = np.random.random((n, 1))

print("iterative")

s = [[-5], [7], [3]]

for _ in range(100):
    x1 -= 0.01*df(x1)
    x1[:len(s)] = s

def solve_constrained(A, b, s):
    k = len(s)
    # solve Ax = b
    # with symmetric A
    # with constraint
    # x[:k] = s
    #
    # decompose A into
    # [          kxk   B =     kx(n-k)]
    # [B.T = (n-k)xk   C = (n-k)x(n-k)]
    # then
    # x = [         s ]
    #     [-B.T.dot(s)]
    B = A[:k, k:]
    C = A[k:, k:]

    x = np.zeros((n, 1))
    x[:k] = s
    x[k:] = -np.linalg.solve(C, B.T.dot(s))

    return x

x3 = solve_constrained(A, b, s)

print("f(x1)")
print(f(x1))
print("f(x3)")
print(f(x3))
