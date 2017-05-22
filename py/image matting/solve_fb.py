import matplotlib.pyplot as plt
import numpy as np
import scipy.sparse
import scipy.misc
import time

np.random.seed(0)

def lstsq(A, b, max_steps, tolerance = 1e-20):
    x = np.zeros(A.shape[1])

    # replace AT with I for solving Ax = b without least square
    AT = A.T
    b = AT@b

    residual = b - AT@(A@x)
    p = residual
    residual_old = np.sum(residual**2)

    for i in range(max_steps):
        q = AT@(A@p)
        alpha = residual_old / np.sum(p*q)

        x += alpha*p
        residual -= alpha*q
        
        residual_new = np.sum(residual**2)

        if residual_new < tolerance:
            print("break after %d iterations"%i)
            break

        p = residual + residual_new/residual_old * p
        
        residual_old = residual_new
    
    return x

def flatten(x):
    return x.reshape(np.product(x.shape))

path = "dandelion"
image = scipy.misc.imread(path + "/image.png")/255.0
trimap = scipy.misc.imread(path + "/trimap.png")/255.0
alpha = scipy.misc.imread(path + "/alpha.png")/255.0

height, width, depth = image.shape
wh = width*height

trimap = trimap[:, :, 0]
alpha = alpha.reshape((height, width, 1))
alphaf = flatten(alpha)

unknown = np.logical_and(trimap > 0.1, trimap < 0.9)
known = np.logical_not(unknown)

def idx(y, x):
    return x + y*width

d = np.ones((height, width))
d[:, 0] = 0
d[0, :] = 0
d = flatten(d)

def debug(d):
    assert(len(d.shape) == 1)
    
    with open("js/debug.js", "w") as f:
        d = list(d)
        d = ",".join(map(str, d))
        f.write("var debug = [" + d + "]")
        
    print("debug data written to disk")
    import sys
    sys.exit(0)

I = scipy.sparse.spdiags(d, 0, wh, wh)
Dx = scipy.sparse.spdiags(-d[-idx(0, -1):], idx(0, -1), wh, wh)
Dy = scipy.sparse.spdiags(-d[-idx(-1, 0):], idx(-1, 0), wh, wh)

W0 = scipy.sparse.spdiags(np.sqrt(np.abs((I + Dx)@alphaf)), 0, wh, wh)
W1 = scipy.sparse.spdiags(np.sqrt(np.abs((I + Dy)@alphaf)), 0, wh, wh)

F = np.zeros(image.shape)
B = np.zeros(image.shape)

for i in range(3):
    color = flatten(image[:, :, i])

    alpha0 = scipy.sparse.spdiags(0 + alphaf, 0, wh, wh)
    alpha1 = scipy.sparse.spdiags(1 - alphaf, 0, wh, wh)

    A = scipy.sparse.bmat([
        [alpha0, alpha1],
        [W0@I + W0@Dx, None],
        [W1@I + W1@Dy, None],
        [None, W0@I + W0@Dx],
        [None, W1@I + W1@Dy],
    ])

    b = np.concatenate([
        color,
        np.zeros(wh*4),
    ])

    t0 = time.clock()
    x = lstsq(A, b, max_steps=100)
    t1 = time.clock()
    print("%f seconds"%(t1 - t0))

    F[:, :, i] = x[:wh].reshape((height, width))
    B[:, :, i] = x[wh:].reshape((height, width))

if 1:
    plt.subplot(2, 2, 1)
    plt.title("Naive method")
    plt.imshow(image*alpha)
    plt.axis('off')

    plt.subplot(2, 2, 2)
    plt.title("Intermediate foreground")
    plt.imshow(F)
    plt.axis('off')

    plt.subplot(2, 2, 3)
    plt.title("Foreground")
    plt.imshow(F*alpha, cmap='Greys_r')
    plt.axis('off')

    plt.show()
