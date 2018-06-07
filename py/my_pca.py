from sklearn.decomposition import PCA
import numpy as np
import numpy.linalg
from sklearn.preprocessing import StandardScaler

np.random.seed(0)

n = 64
p = 64
d = 2

X = np.random.randn(n, p)

if 1:
    X -= np.mean(X, axis=0, keepdims=True)
    X /= np.std(X, axis=0, keepdims=True)
else:
    X = StandardScaler().fit_transform(X)

if 1:
    pca = PCA(n_components=d)
    Y = pca.fit_transform(X)

    print(Y[:2, :2])

if 1:
    cov = np.cov(X.T)
    e, V = np.linalg.eigh(cov)

    assert(np.allclose(cov, V@np.diag(e)@V.T))

    indices = np.argsort(np.abs(e))[::-1]
    V = V[:, indices]
    A = X@V[:, :d]
    print(A[:2, :2])

if 1:
    U, s, VT = np.linalg.svd(X, full_matrices=False)

    Y = X@VT[:d].T

    print(Y[:2, :2])
