import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.contrib.layers import conv2d, conv2d_transpose, batch_norm, linear, bias_add, xavier_initializer
import numpy as np
import time, sys, os
from tensorflow.examples.tutorials.mnist import input_data

def conv(X, num_kernels, kernel_size=3, activation_fn=tf.nn.tanh):
    return conv2d(X, num_kernels, kernel_size, activation_fn=activation_fn)

def convT(X, num_kernels, kernel_size=3, activation_fn=tf.nn.tanh):
    return conv2d_transpose(X, num_kernels, kernel_size, activation_fn=activation_fn)

def flatten(X):
    flat_size = np.prod(X.get_shape().as_list()[1:])
    return tf.reshape(X, [-1, flat_size])

def unflatten(X, shape):
    return tf.reshape(X, [-1] + list(shape))

def avg_pool(X):
    return tf.nn.avg_pool(X, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')

def max_pool(X):
    return tf.nn.max_pool(X, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')

def uppool(X):
    height, width = X.get_shape().as_list()[1:3]
    return tf.image.resize_images(X, (height*2, width*2))

def relu(X, leak=0.1):
    return tf.maximum(leak*X, X)

def lerp(a, b, u):
    return a + u*(b - a)

mnist = input_data.read_data_sets('MNIST_data', one_hot=True)

images = mnist.train.images
labels = mnist.train.labels

height = 28
width  = 28
depth  = 1
num_labels = 10

input_placeholder = tf.placeholder(tf.float32, [None, height, width, depth])
output_placeholder = tf.placeholder(tf.float32, [None, num_labels])

def make_batch(batch_size):
    indices = np.random.randint(len(images), size=batch_size)
    inputs = images[indices].reshape((batch_size, height, width, depth))
    outputs = labels[indices]
    return {
        input_placeholder: inputs,
        output_placeholder: outputs,
    }

X = input_placeholder

for size in [4, 8, 16, 32, 64]:
    X = conv(X, size)
    X = max_pool(X)
    print(X.get_shape().as_list())

X = flatten(X)
for size in [128, 64, 32]:
    X = linear(X, size)
X = linear(X, num_labels, activation_fn=None)
Y = X

loss = tf.reduce_mean(
    tf.nn.softmax_cross_entropy_with_logits(labels=output_placeholder, logits=Y))

correct = tf.equal(tf.argmax(output_placeholder, 1), tf.argmax(Y, 1))
accuracy = tf.reduce_mean(tf.cast(correct, tf.float32))

optimizer = tf.train.AdamOptimizer()
train = optimizer.minimize(loss)

sess = tf.InteractiveSession()
initializer = tf.global_variables_initializer()
sess.run(initializer)

train_losses = []
train_accuracies = []
for batch in range(1000):
    feed_dict = make_batch(100)
    train_loss, train_accuracy, _ = sess.run([loss, accuracy, train], feed_dict=feed_dict)
    train_losses.append(train_loss)
    train_accuracies.append(train_accuracy)

    if batch % 100 == 0:
        print("%05d, %.5f, %.5f, %.5f, %.5f, %7.2f"%(batch, train_loss, train_accuracy, min(train_losses), min(train_accuracies), time.clock()))

