# This is going to be a convolutional layer someday.
# But not today!

import numpy as np
import scipy
import scipy.misc
import scipy.signal
import matplotlib.pyplot as plt
import time
np.random.seed(0)

if 0:
    image = np.random.randn(10, 10, 3).astype(np.float32)
    kernel = np.random.randn(5, 5, 3).astype(np.float32)
    kernel /= np.sum(kernel)
    bias = 0.666

    true_kernel = np.random.rand(*kernel.shape).astype(np.float32)
    true_bias = -0.1337
    true_image = scipy.signal.correlate2d(image, true_kernel, mode='same') + true_bias
else:
    #kernel = np.random.randn(5, 5, 3).astype(np.float32)
    kernel = np.ones((21, 21, 3)).astype(np.float32)
    kernel *= 1.0/np.sum(kernel)
    bias = 0.0
    image = scipy.misc.imread("wheat.bmp").astype(np.float32)/256
    true_image = scipy.misc.imread("wheat_motion_blurred.bmp").astype(np.float32)/256
    #kernel = kernel[:, :, 0:1]
    #image = image[:, :, 0:1]
    #true_image = true_image[:, :, 0:1]

m = np.zeros(kernel.shape, dtype=np.float32)
v = np.zeros(kernel.shape, dtype=np.float32)

for _ in range(10):
    t = time.clock()
    np.fft.ifftn(np.fft.fftn(image[:, :, 0]))
    dt = time.clock() - t
    print(dt*3)

for _ in range(5):
    ry = kernel.shape[0]//2
    rx = kernel.shape[1]//2
    rz = kernel.shape[2]
    padded = np.zeros((image.shape[0] + ry*2, image.shape[1] + rx*2, rz), dtype=np.float32)
    padded[ry:-ry, rx:-rx, :] = image

    t0 = time.clock()
    convolved = np.zeros(image.shape, dtype=np.float32) + bias
    for y in range(kernel.shape[0]):
        for x in range(kernel.shape[1]):
            subimage = padded[y:y+image.shape[0], x:x+image.shape[1], :]
            convolved += kernel[y, x, :]*subimage
    t1 = time.clock()
    
    delta = convolved - true_image
    loss = np.mean(delta**2)

    delta_kernel = np.zeros(kernel.shape, dtype=np.float32)

    t2 = time.clock()
    for y in range(kernel.shape[0]):
        for x in range(kernel.shape[1]):
            subimage = padded[y:y+image.shape[0], x:x+image.shape[1], :]
            delta_kernel[y, x, :] = np.sum(delta*subimage)
    t3 = time.clock()
    
    print("Loss: %e, bias: %f, conv: %f sec, backprop: %f sec"%(loss, bias, t1 - t0, t3 - t2))

    gradient = 1.0/np.product(image.shape)*delta_kernel
    """
    learning_rate = 0.0001
    beta1 = 0.9
    beta2 = 0.999
    eps = 1e-8
    
    m = beta1*m + (1 - beta1)*gradient
    v = beta2*v + (1 - beta2)*gradient**2
    kernel -= learning_rate*m/(np.sqrt(v) + eps)
    """
    learning_rate = 0.01
    kernel -= learning_rate*gradient
    bias -= learning_rate/np.product(image.shape)*np.sum(delta)

def show():
    kernel2 = ((kernel - np.min(kernel))/(np.max(kernel) - np.min(kernel))*255).astype(np.uint8)
    plt.imshow(kernel2)
    plt.show()

show()
"""
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
