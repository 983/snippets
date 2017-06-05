import numpy as np
import matplotlib.pyplot as plt
import scipy.misc

image = scipy.misc.imread("test.png", flatten=True)

image = scipy.misc.imresize(image, 256)

plt.subplot(1, 2, 1)
plt.imshow(image, cmap='gray', interpolation='nearest')


n_angles = max(image.shape)
angles = np.linspace(0, 180, n_angles, endpoint=False)

cols = []
for angle in angles:
    rotated = scipy.misc.imrotate(image, angle)/256.0
    col = np.sum(rotated, axis=1)
    cols.append(col)

transformed = np.column_stack(cols)

plt.subplot(1, 2, 2)

plt.imshow(transformed, cmap='gray', interpolation='nearest')

plt.show()
