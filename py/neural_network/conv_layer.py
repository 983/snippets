# This is going to be a convolutional layer someday.
# But not today!

import numpy as np
np.random.seed(0)
"""
def make_range2d(nx, ny):
    x = np.arange(nx)
    y = np.arange(ny)
    x, y = np.meshgrid(x, y)
    x = x.flatten()
    y = y.flatten()
    return x, y

def make_conv(nx, ny, dx, dy, weights):
    count = len(weights)
    n = nx * ny

    i_inds = np.zeros(n * count, dtype=np.int32)
    j_inds = np.zeros(n * count, dtype=np.int32)
    values = np.zeros(n * count, dtype=np.float64)

    k = 0
    x, y = make_range2d(nx, ny)
    for dx2, dy2, weight in zip(dx, dy, weights):
        x2 = np.clip(x + dx2, 0, w - 1)
        y2 = np.clip(y + dy2, 0, h - 1)
        
        i_inds[k:k + n] = x + y * nx
        j_inds[k:k + n] = x2 + y2 * nx
        values[k:k + n] = weight
        
        k += n

    A = scipy.sparse.csr_matrix((values, (i_inds, j_inds)), shape=(n, n))

    return A

dx, dy = make_range2d(2*r + 1, 2*r + 1)
dx -= r
dy -= r

#A = make_conv(h, w, dx, dy, kernel.flatten())

"""

def make_conv(nx, ny, kernel, r):
    i_inds = []
    j_inds = []
    values = []

    for y in range(ny):
        for x in range(nx):
            i = x + y*nx
            
            for dy in range(-r, r + 1):
                for dx in range(-r, r + 1):
                    x2 = max(0, min(nx - 1, x + dx))
                    y2 = max(0, min(ny - 1, y + dy))
                    
                    j = x2 + y2*nx
                    
                    value = kernel[dy + r, dx + r]
                    
                    i_inds.append(i)
                    j_inds.append(j)
                    values.append(value)

    n = nx * ny
    
    import scipy.sparse
    A = scipy.sparse.csc_matrix((values, (i_inds, j_inds)), shape=(n, n))
    
    return A

def pad(image):
    ny, nx = image.shape[:2]
    
    padded = np.zeros((ny + 2, nx + 2))
    
    # center
    padded[1:1+ny, 1:1+nx] = image
    
    # sides
    padded[1:1+ny, 0] = image[:, 0]
    padded[0, 1:1+nx] = image[0, :]
    padded[1:1+ny, -1] = image[:, -1]
    padded[-1, 1:1+nx] = image[-1, :]
    
    # corners
    padded[0, 0] = image[0, 0]
    padded[-1, -1] = image[-1, -1]
    padded[0, -1] = image[0, -1]
    padded[-1, 0] = image[-1, 0]
    
    return padded

def conv_forward(image, kernel):
    ny, nx = image.shape[:2]
    ky, kx = kernel.shape[:2]
    
    padded = pad(image)
    
    result = np.zeros((ny, nx))
    
    for y in range(ky):
        for x in range(kx):
            result += kernel[y, x] * padded[y:y+ny, x:x+nx]
    
    return result

def conv_backward(image, gradient):
    ny, nx = image.shape[:2]
    
    assert(image.shape == gradient.shape)
    
    ky = 3
    kx = 3
    
    kernel = np.zeros((ky, kx))
    
    padded = pad(image)
    
    for y in range(ky):
        for x in range(kx):
            kernel[y, x] = np.sum(gradient * padded[y:y+ny, x:x+nx])
    
    return kernel

def test_conv():
    ny = 17
    nx = 13
    r = 1

    image = np.random.rand(ny, nx)
    kernel = np.random.randn(2*r + 1, 2*r + 1)
    A = make_conv(nx, ny, kernel, r)

    import scipy.signal
    convolved1 = scipy.signal.correlate2d(image, kernel, mode='same', boundary='symm')
    convolved2 = conv_forward(image, kernel)
    convolved3 = (A @ image.flatten()).reshape(ny, nx)

    assert(np.max(np.abs(convolved1 - convolved2)) < 1e-15)
    assert(np.max(np.abs(convolved1 - convolved3)) < 1e-15)

def test_back():
    ny = 17
    nx = 13
    n = nx * ny
    r = 1
    
    eps = 1e-5
    
    image = np.random.rand(ny, nx)
    true_kernel = np.random.randn(2*r + 1, 2*r + 1)
    
    ground_truth = conv_forward(image, true_kernel)
    
    learning_rate = 0.1
    beta1 = 0.9
    beta2 = 0.999
    eps = 1e-8
    
    weights = np.random.randn(2*r + 1, 2*r + 1)

    m = np.zeros_like(weights)
    v = np.zeros_like(weights)

    for iteration in range(200 + 1):
        
        def calculate_loss(weights):
            kernel = weights
        
            prediction = conv_forward(image, kernel)
        
            loss = np.sum(np.square(prediction - ground_truth))
            
            return loss, prediction
        
        loss, prediction = calculate_loss(weights)
        
        if iteration % 10 == 0:
            print(f"{iteration:4d} - {loss:15.10f}")
        
        def calculate_gradients():
            gradient = 2 * (prediction - ground_truth)
            gradient = conv_backward(image, gradient)
            return gradient
        
        gradients = calculate_gradients()
        
        m = beta1*m + (1 - beta1)*gradients
        v = beta2*v + (1 - beta2)*gradients**2
        weights -= learning_rate*m/(np.sqrt(v) + eps)
    

def main():
    #test_conv()
    test_back()

if __name__ == "__main__":
    main()

"""
if 1:
    image = np.random.randn(10, 10).astype(np.float32)
    kernel = np.random.randn(5, 5).astype(np.float32)
    kernel /= np.sum(kernel)
    bias = 0.666

    true_kernel = np.random.rand(*kernel.shape).astype(np.float32)
    true_bias = -0.1337
    true_image = scipy.signal.correlate2d(image, true_kernel, mode='same') + true_bias
else:
    kernel = np.random.randn(5, 5).astype(np.float32)
    kernel = np.ones((15, 15)).astype(np.float32)
    kernel *= 1.0/np.sum(kernel)
    bias = 0.0
    image = scipy.misc.imread("wheat.bmp", flatten=True).astype(np.float32)/256
    true_image = scipy.misc.imread("wheat_blurred.bmp", flatten=True).astype(np.float32)/256

m = np.zeros(kernel.shape, dtype=np.float32)
v = np.zeros(kernel.shape, dtype=np.float32)

for _ in range(100):
    ry = kernel.shape[0]//2
    rx = kernel.shape[1]//2
    padded = np.zeros((image.shape[0] + ry*2, image.shape[1] + rx*2), dtype=np.float32)
    padded[ry:-ry, rx:-rx] = image
    
    convolved = np.zeros(image.shape, dtype=np.float32) + bias
    for y in range(kernel.shape[0]):
        for x in range(kernel.shape[1]):
            convolved += kernel[y, x]*padded[y:y+image.shape[0], x:x+image.shape[1]]

    delta = convolved - true_image
    loss = np.mean(delta**2)

    print("Loss: %e, bias: %f"%(loss, bias))

    delta_kernel = np.zeros(kernel.shape, dtype=np.float32)
    
    for y in range(kernel.shape[0]):
        for x in range(kernel.shape[1]):
            delta_kernel[y, x] = np.sum(delta*padded[y:y+image.shape[0], x:x+image.shape[1]])

    gradient = 1.0/np.product(image.shape)*delta_kernel

    learning_rate = 0.001
    beta1 = 0.9
    beta2 = 0.999
    eps = 1e-8
    
    #m = beta1*m + (1 - beta1)*gradient
    #v = beta2*v + (1 - beta2)*gradient**2
    #kernel -= learning_rate*m/(np.sqrt(v) + eps)
    
    learning_rate = 1.0
    kernel -= learning_rate*gradient
    bias -= learning_rate/np.product(image.shape)*np.sum(delta)

plt.subplot(2, 2, 1)
plt.imshow(image, cmap='gray')
plt.subplot(2, 2, 2)
plt.imshow(convolved, cmap='gray')
plt.subplot(2, 2, 3)
plt.imshow(kernel, cmap='gray')
plt.subplot(2, 2, 4)
plt.imshow(true_image, cmap='gray')
plt.show()
"""
