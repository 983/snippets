import numpy as np
from tensorflow.examples.tutorials.mnist import input_data

mnist = input_data.read_data_sets("mnist", one_hot=True)

train_images = mnist.train.images
train_labels = mnist.train.labels
test_images = mnist.test.images
test_labels = mnist.test.labels

np.save("mnist/train_images.npy", train_images)
np.save("mnist/train_labels.npy", train_labels)
np.save("mnist/test_images.npy", test_images)
np.save("mnist/test_labels.npy", test_labels)
