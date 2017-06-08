import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np
import scipy.misc
import time
import os

def flatten(x):
    return np.reshape(x, np.product(x.shape))

def load_image(path):
    image = scipy.misc.imread(path)
    image = scipy.misc.imresize(image, (224, 224))
    image = image.astype(np.float32)

    r = image[:, :, 0] - 103.939
    g = image[:, :, 1] - 116.779
    b = image[:, :, 2] - 123.68

    bgr_image = np.stack([b, g, r], axis=2)

    return bgr_image

with open("synset.txt", "r") as f:
    label_names = f.read().strip().split("\n")

print("loaded label names")

d = np.load("vgg19.npy", encoding='latin1').item()

print("loaded weights")

def conv_layer(X, name):
    with tf.variable_scope(name):
        filters = tf.constant(d[name][0], name="filters")
        biases = tf.constant(d[name][1], name="biases")
        
        X = tf.nn.conv2d(X, filters, [1,1,1,1], padding='SAME')
        X = tf.nn.bias_add(X, biases)
        X = tf.nn.relu(X)
        
        return X

def fc_layer(X, name):
    with tf.variable_scope(name):
        weights = tf.constant(d[name][0], name="filters")
        biases = tf.constant(d[name][1], name="biases")

        shape = X.get_shape().as_list()
        X = tf.reshape(X, [-1, np.product(shape[1:])])
        X = tf.matmul(X, weights)
        X = tf.nn.bias_add(X, biases)

        return X

def max_pool(X, name):
    return tf.nn.max_pool(X, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME', name=name)

input_placeholder = tf.placeholder(np.float32, shape=[None, 224, 224, 3])
X = input_placeholder

X = conv_layer(X, "conv1_1")
X = conv_layer(X, "conv1_2")
X = max_pool(X, "pool1")

X = conv_layer(X, "conv2_1")
X = conv_layer(X, "conv2_2")
X = max_pool(X, "pool2")

X = conv_layer(X, "conv3_1")
X = conv_layer(X, "conv3_2")
X = conv_layer(X, "conv3_3")
X = conv_layer(X, "conv3_4")
X = max_pool(X, "pool3")

X = conv_layer(X, "conv4_1")
X = conv_layer(X, "conv4_2")
X = conv_layer(X, "conv4_3")
X = conv_layer(X, "conv4_4")
X = max_pool(X, "pool4")

X = conv_layer(X, "conv5_1")
X = conv_layer(X, "conv5_2")
X = conv_layer(X, "conv5_3")
X = conv_layer(X, "conv5_4")
X = max_pool(X, "pool5")

X = fc_layer(X, "fc6")
X = tf.nn.relu(X)

X = fc_layer(X, "fc7")
X = tf.nn.relu(X)

X = fc_layer(X, "fc8")

X = tf.nn.softmax(X, name="prob")

Y = X

print("created network")

dirname = "test_data"
paths = [os.path.join(dirname, path) for path in os.listdir(dirname)]
batch = [load_image(path) for path in paths]

print("loaded images")

sess = tf.InteractiveSession()
#initializer = tf.global_variables_initializer()
#sess.run(initializer)

for _ in range(1):
    start_time = time.clock()
    
    results = sess.run(Y, feed_dict={input_placeholder:batch})

    delta_time = time.clock() - start_time

    print("predicted labels, %f seconds"%delta_time)

    for i, result in enumerate(results):
        class_id = np.argmax(result)
        print("%30s label: %s"%(paths[i], label_names[class_id]))
