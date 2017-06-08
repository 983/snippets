import autograd.numpy as np
from autograd import grad

import time
import scipy.signal
import scipy.optimize
import matplotlib.pyplot as plt

def flatten(x):
    return np.reshape(x, np.product(x.shape))
"""
kernel = np.random.rand(3, 3).astype(np.float32)
image = np.random.rand(224, 224).astype(np.float32)

for _ in range(10):
    t = time.clock()
    scipy.signal.correlate2d(image, kernel)
    dt = time.clock() - t
    print("%f mflops"%(1e-6/dt*np.product(kernel.shape)*np.product(image.shape)))
"""

def get_scale_and_bias(x, y):
    # return a, b such that (a*x + b - y)**2 is minimized
    
    sum_xx = np.sum(x*x)
    sum_x  = np.sum(x)
    sum_xy = np.sum(x*y)
    sum_y  = np.sum(y)

    def inv2(a, b, c, d):
        return 1.0/(a*d - b*c)*np.array([[d, -b], [-c, a]])

    c = inv2(sum_xx, sum_x, sum_x, len(x))@np.array([sum_xy, sum_y])

    return c
"""
def loss(parameters):
    a = parameters[0:3]
    b = parameters[3:6]
    less_kernels = parameters[6:6+9*3]

    result = 0
    
    for kernel in kernels:
        difference = kernel
        for less_kernel in less_kernels:
            
            difference = kernel - 
            result += 
"""
#x = np.random.rand(10)
#print(get_scale_and_bias(x, 7*x + 11))

def flatten(x):
    return np.reshape(x, np.product(x.shape))

if 1:
    """
    d = np.load("vgg19.npy", encoding='latin1').item()

    items = sorted(d.items())

    if 0:
        for key, (W, b) in items:
            print("%10s %16s %8s"%(key, str(W.shape), str(b.shape)))

    width = 224
    height = 224

    key, (W, b) = items[2]

    n = W.shape[-1]

    W = [flatten(W[:, :, 1, i]) for i in range(n)]

    A = np.array(W).astype(np.float64)

    kernels = A
    
    np.save("kernels.npy", kernels)
    """

    kernels = np.load("kernels.npy")

    n_kernels = len(kernels)
    n_less = 4

    def decompose(parameters):
        less_kernels = parameters[:n_less*9].reshape((n_less, 9))
        factors = parameters[n_less*9:].reshape((n_kernels, n_less))
        
        return less_kernels, factors

    def loss(parameters):
        less_kernels, factors = decompose(parameters)

        """
        loss = 0
        for i, kernel in enumerate(kernels):
            difference = kernel.copy()
            for j, less_kernel in enumerate(less_kernels):
                difference = difference - factors[i, j]*less_kernel
            
            #difference = kernel - factors[i]@less_kernels
            
            loss += np.sum(difference*difference)
        """
        difference = kernels - factors@less_kernels
        loss = np.sum(difference*difference)
        
        return loss

    grad_loss = grad(loss)

    for _ in range(10):
        less_kernels = np.random.rand(n_less, 9)
        factors = np.random.rand(n_kernels, n_less)
        parameters = np.concatenate([flatten(less_kernels), flatten(factors)])
        #result = scipy.optimize.fmin_bfgs(loss, parameters, grad_loss, maxiter=50)
        parameters = scipy.optimize.fmin_cg(loss, parameters, grad_loss, maxiter=500)

        print("loss: %f"%loss(parameters))

    less_kernels, factors = decompose(parameters)

    for i in range(n_less):
        plt.subplot(1, n_less, 1 + i)
        plt.imshow(less_kernels[i].reshape((3, 3)), cmap='gray')
        plt.axis('off')
    plt.show()
    #print(result)
    """
    rate = 0.01
    last_loss = 1e10
    for _ in range(100):
        current_loss = loss(parameters)
        print(current_loss, rate)
            
        parameters -= rate*grad_loss(parameters)

        if abs(current_loss - last_loss) < 1e-5:
            rate *= 0.9
            
        last_loss = current_loss
    """
    """
    #W.append(np.ones_like(W[0]))


    U, s, V = np.linalg.svd(A, full_matrices=False)
    S = np.diag(s)

    D = np.abs(A - U@S@V)
    assert(np.max(D) < 1e-10)

    s[4:] = 0
    
    S = np.diag(s)
    D = np.abs(A - U@S@V)
    print(np.max(D))
    """
    """
    errors = np.zeros((n, n))
    
    for i in range(n):
        best = (1e10, None, None, None, None)
        
        for j in range(n):
            if i == j: continue
            
            x = W[i]
            y = W[j]
            
            a, b = get_scale_and_bias(x, y)
            
            v = a*x + b - y
            
            error = np.sum(v*v)

            best = min(best, (error, i, j, a, b))

            errors[i, j] = error
        
        print(best)
    
    plt.matshow(errors, cmap='gray')
    plt.show()
    """

    """
    for i in range(64):
        plt.subplot(8, 8, 1 + i)
        plt.imshow(W[:, :, :, i])
        plt.axis('off')
    plt.show()
    """
