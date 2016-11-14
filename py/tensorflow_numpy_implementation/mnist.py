import struct
import numpy as np

print("MNIST dataset should be downloaded to mnist folder")

def _load(image_path, label_path):
    with open(image_path, "rb") as f:
        magic_number, n_images, image_width, image_height = struct.unpack(">IIII", f.read(16))
        assert(magic_number == 2051)
        images = np.frombuffer(f.read(), dtype=np.uint8)
        images = images.reshape((n_images, image_width*image_height))
        images = (1.0/256.0)*images.astype(np.float32)

    with open(label_path, "rb") as f:
        magic_number, n_labels = struct.unpack(">II", f.read(8))
        assert(magic_number == 2049)
        labels = np.frombuffer(f.read(), dtype=np.uint8)
        one_hot_labels = np.eye(10)[labels]
    
    return (images, one_hot_labels)

def load_train():
    return _load("mnist/train-images.idx3-ubyte", "mnist/train-labels.idx1-ubyte")

def load_test():
    return _load("mnist/t10k-images.idx3-ubyte", "mnist/t10k-labels.idx1-ubyte")

"""
images, labels = load_train()

from matplotlib import pyplot as plt

n = 9
fig = plt.figure()
for i in range(n):
    image = images[i, :].reshape(28, 28)
    subplot = fig.add_subplot(1, n, 1 + i)
    subplot.set_title("%d"%np.argmax(labels[i]))
    plt.imshow(image, cmap='gray')
    plt.axis('off')
plt.show()
"""
