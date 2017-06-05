import matplotlib.pyplot as plt
import numpy as np
from numba import jit
import scipy.misc
import scipy.sparse.linalg

image = scipy.misc.imread("test_simple.png", flatten=True)
n = image.shape[0]

@jit(nopython=True, nogil=True)
def checked_add(R, Ri, n, w, ix, iy):
    if ix >= 0 and ix < n and iy >= 0 and iy < n:
        R[Ri, iy*n + ix] += w
        
@jit(nopython=True, nogil=True)
def make_R(n):
    R = np.zeros((n*n, n*n))
    Ri = 0
    
    center_x = (n + 1)*0.5
    center_y = (n + 1)*0.5

    for i in range(n):
        for j in range(n):
            for k in range(n):
                x = k + 0.5
                y = i + 0.5
                angle = np.pi/n*j
                
                co = np.cos(angle)
                si = np.sin(angle)

                xc = x - center_x
                yc = y - center_y
                
                xr = co*xc - si*yc + center_x
                yr = si*xc + co*yc + center_y

                ix = int(np.floor(xr))
                iy = int(np.floor(yr))

                if 1:
                    ux = xr - ix
                    uy = yr - iy

                    checked_add(R, Ri, n, (1 - ux)*(1 - uy)/n, ix + 0, iy + 0)
                    checked_add(R, Ri, n, (    ux)*(1 - uy)/n, ix + 1, iy + 0)
                    checked_add(R, Ri, n, (1 - ux)*(    uy)/n, ix + 0, iy + 1)
                    checked_add(R, Ri, n, (    ux)*(    uy)/n, ix + 1, iy + 1)
                else:
                    checked_add(R, Ri, n, 1.0/n, ix, iy)

            Ri += 1
    
    return R

R = make_R(n)
print("R density:", np.sum(R != 0)/np.product(R.shape))

transformed = np.dot(R, image.reshape(n*n))
transformed_noise = transformed + np.random.randn(*transformed.shape)*5
reconstructed = np.linalg.lstsq(R, transformed)[0]
reconstructed_noise = scipy.sparse.linalg.lsqr(R, transformed_noise, damp=0.0)[0]

plt.subplot(2, 3, 1)
plt.title('$\mathbf{I}$')
plt.imshow(image, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 2)
plt.title('$\mathbf{R}^{-1} \mathbf{R} \mathbf{I}$')
plt.imshow(reconstructed.reshape((n, n)), cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 3)
plt.title('$\mathbf{R}^{-1} ( \mathbf{R} \mathbf{I} + noise )$')
plt.imshow(reconstructed_noise.reshape((n, n)), cmap='gray', interpolation='nearest')
plt.axis('off')


plt.subplot(2, 3, 4)
plt.title('$\mathbf{R}$')
plt.imshow(R, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 5)
plt.title('$\mathbf{R} \mathbf{I}$')
plt.imshow(transformed.reshape((n, n)), cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(2, 3, 6)
plt.title('$\mathbf{R} \mathbf{I} + noise $')
plt.imshow(transformed_noise.reshape((n, n)), cmap='gray', interpolation='nearest')
plt.axis('off')
plt.show()
