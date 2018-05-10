import numpy as np
import time

inv = np.linalg.inv

# 2.3728639

def lu(M):
    n2 = M.shape[0]
    
    n = n2//2
    
    A = M[:n, :n]
    B = M[:n, n:]
    C = M[n:, :n]
    D = M[n:, n:]

    L = np.eye(n2)
    U = np.eye(n2)

    CinvA = C@inv(A)

    L[n:, :n] = CinvA
    
    U[:n, :n] = A
    U[:n, n:] = B
    U[n:, n:] = D - CinvA@B

    return L, U

for _ in range(10):
    n = 128
    np.random.seed(0)
    A = np.random.rand(n, n)

    L,U = lu(A)

    print(np.max(np.abs(A - L@U)))
