import numpy as np
import time

# 2.3728639

def inv(M):
    n2 = M.shape[0]
    
    if n2 <= 64: return np.linalg.inv(M)
    
    n = n2//2
    
    A = M[:n, :n]
    B = M[:n, n:]
    C = M[n:, :n]
    D = M[n:, n:]

    result = np.zeros_like(M)

    invA = inv(A)
    CinvA = C@invA
    S = inv(D - CinvA@B)
    invABS = invA@B@S

    result[:n, :n] = invA + invABS@CinvA
    result[:n, n:] = -invABS
    result[n:, :n] = -S@CinvA
    result[n:, n:] = S
    
    return result

def inv_inplace(M):
    n2 = M.shape[0]
    
    if n2 <= 64:
        M[...] = np.linalg.inv(M)
        return
    
    n = n2//2
    
    A = M[:n, :n].copy()
    B = M[:n, n:].copy()
    C = M[n:, :n].copy()
    D = M[n:, n:].copy()

    invA = inv(A)
    CinvA = C@invA
    S = inv(D - CinvA@B)
    invABS = invA@B@S

    M[:n, :n] = invA + invABS@CinvA
    M[:n, n:] = -invABS
    M[n:, :n] = -S@CinvA
    M[n:, n:] = S

for _ in range(10):
    n = 1024
    np.random.seed(0)
    A = np.random.rand(n, n)

    t = time.clock()

    if 1:
        invA = A.copy()
        inv_inplace(invA)
    else:
        invA = inv(A)

    dt = time.clock() - t

    print(dt, np.max(np.abs(np.eye(n) - A@invA)))
