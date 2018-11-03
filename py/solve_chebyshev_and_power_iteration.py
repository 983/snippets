import numpy as np
from numpy.linalg import eigvals, eigh, eigvalsh, solve
import scipy.sparse.linalg

np.random.seed(0)

n = 50
A = np.random.rand(n, n)
A += A.T
A += n*np.eye(n)
#A = A @ np.diag(1/np.diag(A))

def power_iteration(A, num_simulations):
    # Ideally choose a random vector
    # To decrease the chance that our vector
    # Is orthogonal to the eigenvector
    b_k = np.random.rand(A.shape[1])

    for _ in range(num_simulations):
        # calculate the matrix-by-vector product Ab
        b_k1 = np.dot(A, b_k)

        # calculate the norm
        b_k1_norm = np.linalg.norm(b_k1)
        
        # re normalize the vector
        b_k = b_k1 / b_k1_norm

    return b_k1_norm, b_k

counter = 0
def A_dot(x):
    global counter
    counter += 1
    return A.dot(x)

A_dot = scipy.sparse.linalg.LinearOperator((n, n), A_dot)

counter = 0
print(scipy.sparse.linalg.eigsh(A_dot, k=1)[0])
print("%d calls to A_dot"%counter)
print("")

counter = 0
print(scipy.sparse.linalg.lobpcg(
    A_dot,
    np.random.randn(n, 1))[0])
print("%d calls to A_dot"%counter)
print("")

if 1:
    print("power_iteration:", power_iteration(A, 20)[0])
    print("")
    w = eigvalsh(A)

    lMin = w.min()
    lMax = w.max()

print("smallest eigenvalue:", lMin)
print("largest eigenvalue:", lMax)
print("")

def solve_chebyshev(A, b, lMin, lMax, x=None, M=None):
        
    d = (lMax + lMin)/2
    c = (lMax - lMin)/2

    M = np.eye(n)
    # TODO does not converge with jacobi preconditioner
    # do eigenvalues have to be adjusted?
    #M = np.diag(1/np.diag(A))

    if x is None:
        x = np.random.randn(n)

    r = b - A.dot(x)

    for iteration in range(100):
        z = solve(M, r)
        
        if iteration == 0:
            p = z
            alpha = 1/d
        else:
            beta = (c*alpha/2)**2
            alpha = 1/(d - beta/alpha)
            
            p = z + beta*p
        
        x = x + alpha*p
        
        # r = r - alpha * A.dot(p)
        r = b - A.dot(x)
        
        error = np.linalg.norm(r)
        
        print("%05d - %20.16f"%(iteration, error))
        
        if error < 1e-15:
            break
    
    return x

b = np.random.randn(n)

x = solve_chebyshev(A, b, lMin, lMax)

assert(np.linalg.norm(x - solve(A, b)) < 1e-10)
