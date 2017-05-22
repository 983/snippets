import time
import numpy as np
import matplotlib.pyplot as plt
from skimage.io import imread
from skimage.transform import radon, iradon, iradon_sart

image = imread("test.png", as_grey=True)

# more angles reduce error for iradon_sart, but not iradon
m = max(image.shape)
theta = np.linspace(0, 180, m, endpoint=False)

transformed = radon(image, theta=theta, circle=True)

transformed += np.random.randn(*transformed.shape)

t0 = time.clock()
# interpolation='cubic' is 0.02 rms error better but 350 times slower
reconstructed = iradon(transformed, theta=theta, circle=True)
t1 = time.clock()
reconstructed2 = None
n = 10
for _ in range(n):
    reconstructed2 = iradon_sart(transformed, theta=theta, image=reconstructed2, clip=[0, 1])
t2 = time.clock()

print("seconds:")
print(t1 - t0)
print((t2 - t1)/n, "times", n)
print("")

def print_errors(difference):
    print("max error:", np.max(np.abs(difference)))
    print("rms error:", np.sqrt(np.mean(np.square(difference))))
    print("")

print_errors(image - reconstructed)
print_errors(image - reconstructed2)

plt.subplot(1, 4, 1)
plt.imshow(image, cmap='gray', vmin=0, vmax=1, interpolation='nearest')
plt.axis('off')

plt.subplot(1, 4, 2)
plt.imshow(transformed, cmap='gray')
plt.axis('off')
plt.xlabel("angle (deg)")

plt.subplot(1, 4, 3)
plt.imshow(reconstructed, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(1, 4, 4)
plt.imshow(reconstructed2, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.show()
