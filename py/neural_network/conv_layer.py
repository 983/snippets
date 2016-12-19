# This is going to be a convolutional layer someday.
# But not today!

import numpy as np
import scipy
import scipy.misc
import scipy.signal
import matplotlib.pyplot as plt
np.random.seed(0)

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
