import struct
import numpy as np
import os
import gzip

try:
    # Python 2
    from urllib import urlretrieve
except:
    # Python 3
    from urllib.request import urlretrieve

MNIST_DIRECTORY = "mnist"

WEBSITE = "http://yann.lecun.com/exdb/mnist/"

TRAIN_FILE_NAMES = [
    "train-images.idx3-ubyte",
    "train-labels.idx1-ubyte",
]

TEST_FILE_NAMES = [
    "t10k-images.idx3-ubyte",
    "t10k-labels.idx1-ubyte",
]

# download mnist things if they are not there yet
for file_name in TRAIN_FILE_NAMES + TEST_FILE_NAMES:
    
    path = MNIST_DIRECTORY + "/" + file_name
    
    if not os.path.isfile(path):
        url = WEBSITE + file_name.replace(".", "-") + ".gz"
        compressed_file_name = MNIST_DIRECTORY + "/" + file_name + ".gz"
        
        print("Downloading %s"%url)
        
        urlretrieve(url, compressed_file_name)
        
        with gzip.open(compressed_file_name, 'rb') as f:
            data = f.read()
        
        with open(path, "wb") as f:
            f.write(data)
        
        print("Decompressed to %s"%path)

def _load(image_path, label_path):
    
    with open(image_path, "rb") as f:
        magic_number, n_images, image_width, image_height = struct.unpack(">IIII", f.read(16))
        assert(magic_number == 2051)
        images = np.frombuffer(f.read(), dtype=np.uint8)
        images = images.reshape((n_images, image_width*image_height))
        images = images.astype(np.float32)
        images = (1.0/256.0)*images

    with open(label_path, "rb") as f:
        magic_number, n_labels = struct.unpack(">II", f.read(8))
        assert(magic_number == 2049)
        labels = np.frombuffer(f.read(), dtype=np.uint8)
        one_hot_labels = np.eye(10)[labels]
        one_hot_labels = one_hot_labels.astype(np.float32)
    
    return (images, one_hot_labels)

TRAIN_DATA, TRAIN_LABELS = _load(MNIST_DIRECTORY + "/" + TRAIN_FILE_NAMES[0], MNIST_DIRECTORY + "/" + TRAIN_FILE_NAMES[1])
TEST_DATA, TEST_LABELS = _load(MNIST_DIRECTORY + "/" + TEST_FILE_NAMES[0], MNIST_DIRECTORY + "/" + TEST_FILE_NAMES[1])
