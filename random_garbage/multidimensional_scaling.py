import numpy as np
import matplotlib.pyplot as plt
import scipy.sparse.linalg
import scipy.linalg
import numpy.linalg
import time
from numba import jit

def annotate(point, text):
    plt.annotate(
            text,
            xy=point, xytext=(-20, 20),
            textcoords='offset points', ha='right', va='bottom',
            bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
            arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

np.random.seed(1337)

n = 500
# dimension of points
m = 2

p = np.random.rand(n, m)*2 - 1

x = p[:, 0]
y = p[:, 1]

plt.subplot(1, 2, 1)
plt.plot(x, y, 'ro')
r = 1.5
plt.xlim(-r, r)
plt.ylim(-r, r)
r = 2
for i in range(3):
    annotate(p[i], "point %d"%i)

@jit(nopython=True, nogil=True)
def get_squared_distance_matrix(p):
    D = np.zeros((n, n))

    for i in range(n):
        for j in range(n):
            d = p[i] - p[j]
            D[i, j] = np.sum(d*d)

    return D

D1 = get_squared_distance_matrix(p)

# centering matrix
J = np.eye(n) - 1/n*np.ones((n, n))

# centering of D
B = -0.5*J@D1@J

def get_largest(eigenvalues_and_eigenvectors):
    eigenvalues, eigenvectors = eigenvalues_and_eigenvectors

    idx = eigenvalues.argsort()[::-1]
    idx = idx[:m]
    eigenvalues = eigenvalues[idx]
    eigenvectors = eigenvectors[:, idx]

    eigenvalues = np.real(eigenvalues)
    eigenvectors = np.real(eigenvectors)

    return eigenvalues, eigenvectors

t0 = time.clock()
eigenvalues1, eigenvectors1 = get_largest(scipy.sparse.linalg.eigsh(B, k=m))
t1 = time.clock()
eigenvalues2, eigenvectors2 = get_largest(np.linalg.eig(B))
t2 = time.clock()
eigenvalues3, eigenvectors3 = get_largest(scipy.linalg.eigh(B, eigvals=(0, n-1)))
t3 = time.clock()

print("seconds:")
print(t1 - t0)
print(t2 - t1)
print(t3 - t2)
print("")
print("errors:")
print(np.linalg.norm(eigenvalues1 - eigenvalues2))
print(np.linalg.norm(eigenvalues1 - eigenvalues3))
print(np.linalg.norm(eigenvalues2 - eigenvalues3))
print(np.linalg.norm(eigenvectors1 - eigenvectors2))
print(np.linalg.norm(eigenvectors1 - eigenvectors3))
print(np.linalg.norm(eigenvectors2 - eigenvectors3))

E = eigenvectors1
L = np.diag(np.sqrt(eigenvalues1))

X = E@L

x = X[:, 0]
y = X[:, 1]

D2 = get_squared_distance_matrix(X)

print("")
print("distance matrix max difference:")
print(np.max(np.abs(D1 - D2)))

plt.subplot(1, 2, 2)
plt.plot(x, y, 'bo')
for i in range(3):
    annotate(X[i], "point %d"%i)
plt.xlim(-r, r)
plt.ylim(-r, r)

plt.show()
