import numpy as np
import struct

def load_images(path):

    with open(path, "rb") as f:
        data = f.read()
    
    magic_number, n_images, height, width = struct.unpack(">IIII", data[:4*4])
    pixels = data[4*4:]
    
    assert(magic_number == 2051)
    assert(len(pixels) == n_images*height*width)

    images = np.frombuffer(pixels, dtype=np.uint8).reshape(n_images, height, width)

    return images

def load_labels(path):
    with open(path, "rb") as f:
        data = f.read()
    
    magic_number, n_labels = struct.unpack(">II", data[:2*4])
    labels = np.frombuffer(data[2*4:], dtype=np.uint8)
    
    assert(magic_number == 2049)
    assert(len(labels) == n_labels)

    return labels

test_images = load_images("t10k-images.idx3-ubyte")
train_images = load_images("train-images.idx3-ubyte")
test_labels = load_labels("t10k-labels.idx1-ubyte")
train_labels = load_labels("train-labels.idx1-ubyte")

mnist = {
    "train_images": train_images,
    "train_labels": train_labels,
    "test_images": test_images,
    "test_labels": test_labels,
}

np.save("mnist", mnist)
