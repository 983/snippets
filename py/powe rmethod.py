import numpy as np
import numpy.linalg
import scipy.sparse.linalg

np.random.seed(0)

print("true eigenvalues:")
n = 4
A = np.random.rand(n, n) - np.eye(n)*n**2

s, V = scipy.sparse.linalg.eigs(A, k=2)
print(s)
s, V = np.linalg.eig(A@A.T)
# Vinv = V if A is symmetric
Vinv = np.linalg.inv(V)
print(np.sort(s)[::-1])

S = np.diag(s)
assert(np.max(np.abs(A@A.T - V@S@Vinv)) < 1e-10)

U, s, VT = np.linalg.svd(A, full_matrices=False)
print(s*s)
S = np.diag(s)

assert(np.max(np.abs(A - U@S@VT)) < 1e-10)

print("\n")

x = np.random.rand(n)
for _ in range(1000):
    x = A@A.T@x
    x /= np.linalg.norm(x)

y = np.random.rand(n)
for _ in range(1000):
    y = A.T@A@x
    y /= np.linalg.norm(y)

print("power method eigenvector:")
print(x)
print(VT[0, :])
print("corresponding eigenvalue (not sure which one):")
print(np.linalg.norm(A.T@A@x))
print(np.linalg.norm(A@A.T@x))
print(np.linalg.norm(A.T@A@y))
print(np.linalg.norm(A@A.T@y))

print("\n")

print("inverse power method:")

n = 100
A = np.random.rand(n)
A = A + A.T + np.eye(n)*n

x = np.ones(n)/n

for _ in range(3):
    u = np.linalg.norm(x)
    x = np.linalg.inv(A)@x/u
    print(u)

v = x/u

print("should be:", np.min(1/np.linalg.eig(A)[0]))
