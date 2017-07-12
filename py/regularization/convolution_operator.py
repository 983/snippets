import numpy as np
import scipy.sparse.linalg
import scipy.misc
import scipy.signal
import scipy.optimize
import matplotlib.pyplot as plt
import time

image = 1.0/256.0*scipy.misc.imread("test.png", flatten=True)
n = image.shape[0]

r = 3
x = np.linspace(-2, 2, 2*r + 1)
weights = np.exp(-x*x)
#weights = np.array([[1, 2, 4, 2, 1]])
weights = 1.0/np.sum(weights)*weights
weights = weights.reshape((len(weights), 1)).dot(weights.reshape((1, len(weights))))

if 0:
    plt.imshow(weights, cmap='gray')
    plt.show()
    import sys
    sys.exit(0)

#b = scipy.signal.convolve2d(image, weights, mode='same', boundary='fill')

values = []
i_indices = []
j_indices = []
#A = np.zeros((n**2, n**2))
for i in range(n):
    for j in range(n):
        for di in range(-r, r + 1):
            for dj in range(-r, r + 1):
                i2 = i + di
                j2 = j + dj
                
                if i2 >= 0 and i2 < n and j2 >= 0 and j2 < n:
                    weight = weights[di + r, dj + r]
                    
                    #A[i*n + j, i2*n + j2] = weight
                    i_indices.append(i*n + j)
                    j_indices.append(i2*n + j2)
                    values.append(weight)

A = scipy.sparse.csr_matrix((values, (i_indices, j_indices)), shape=(n**2, n**2))

def solve_landweber(A, y, magic_constant):
    
    x = np.zeros(n*n)
    for _ in range(1000):
        d = A.T.dot(x) - y
        loss = np.sum(d*d)/n**2
        if loss < 1e-5:
            print("loss:", loss)
            break
        x -= magic_constant*A.T.dot(d)
    return x

def solve_tikhonov(A, y, magic_constant):
    B = A.T.dot(A) + magic_constant**2*scipy.sparse.eye(n**2)
    return scipy.sparse.linalg.spsolve(B, A.T.dot(y))

def solve_truncated_svd(A, y, magic_constant):
    # Assume (usually, U S V.T, but not here)
    # U S V = A
    # least square:
    # inv(A.T A) A.T y
    # inv((V.T S U.T) (U S V)) (V.T S U.T) y
    # inv(V.T S^2 V) (U S V) y
    # inv(V) inv(S^2) inv(V.T) (V.T S U.T) y
    # V.T S^-2 S U.T y
    # V.T inv(S) U.T y

    n = min(A.todense().shape[0], int(magic_constant))

    t = time.clock()    
    U, s, V = np.linalg.svd(A.todense())
    #U, s, V = scipy.sparse.linalg.svds(A, k=n)
    dt = time.clock() - t
    print("%f seconds"%dt)
    
    inv_s = np.zeros_like(s)
    inv_s[:n] = 1/s[:n]

    if 0:
        plt.plot(s)
        plt.show()
    
    return V.T.dot(np.diag(inv_s)).dot(U.T).dot(y)

def solve_regularized(A, y, magic_constant):
    return solve_truncated_svd(A, y, magic_constant)
    return solve_landweber(A, y, magic_constant)
    return solve_tikhonov(A, y, magic_constant)
    return scipy.sparse.linalg.lsqr(A, y, magic_constant)[0]

convolved = A.dot(image.reshape(n*n)).reshape((n, n))

deconvolved = scipy.sparse.linalg.spsolve(A, convolved.reshape(n*n)).reshape((n, n))

convolved_noisy = convolved + np.random.randn(n, n)*0.01

def get_error(guess):
    magic_constant = guess[0]

    deconvolved_noisy = solve_regularized(
        A,
        convolved_noisy.reshape(n*n),
        magic_constant
    ).reshape((n, n))
    
    difference = image - deconvolved_noisy
    error = np.sum(difference*difference)/n**2
    print("error:", error, "magic_constant:", magic_constant)
    return error

#result = scipy.optimize.fmin(func=get_error, x0=0, maxfun=50)
#best_magic_constant = result[0]

# landweber
best_magic_constant = 0.035111
# scipy
best_magic_constant = 0.034476
# tikhonov
best_magic_constant =  0.0335625
# svd
best_magic_constant = 500

deconvolved_noisy = solve_regularized(A, convolved_noisy.reshape(n*n), best_magic_constant).reshape((n, n))

plt.subplot(2, 3, 1)
plt.imshow(image, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 2)
plt.imshow(convolved, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 3)
plt.imshow(convolved_noisy, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 5)
plt.imshow(deconvolved, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 6)
plt.imshow(deconvolved_noisy, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.show()

