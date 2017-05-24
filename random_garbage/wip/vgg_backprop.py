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

def network(X):

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

    return X

Y1 = network(input_placeholder)

print("created network")

image = load_image("data/tiger.jpeg")

batch = [image]

variable_batch = tf.Variable(tf.random_uniform([1, 224, 224, 3]), name='peter')

Y2 = network(variable_batch)

difference = Y1 - Y2
loss = tf.reduce_mean(difference*difference)

optimizer = tf.train.AdamOptimizer()
minimize_loss = optimizer.minimize(loss)

sess = tf.InteractiveSession()
initializer = tf.global_variables_initializer()
sess.run(initializer)

for i in range(10):
    lss, _ = sess.run([loss, minimize_loss], feed_dict={input_placeholder:batch})
    print(lss)

if 1:
    image = sess.run(variable_batch)
    image = image.reshape((224, 224, 3))
    plt.imshow(image)
    plt.show()
