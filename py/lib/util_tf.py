import tensorflow as tf
from tensorflow.contrib.layers import variance_scaling_initializer, conv2d, conv2d_transpose, linear
import numpy as np

def pool(X):
    return tf.nn.max_pool(X, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')

def uppool(X):
    height, width = X.get_shape().as_list()[1:3]
    return tf.image.resize_images(X, (height*2, width*2))

def flatten(X):
    flat_size = np.prod(X.get_shape().as_list()[1:])
    return tf.reshape(X, [-1, flat_size])

def initializer(n_in, n_out):
    W = variance_scaling_initializer()((n_in, n_out))
    W = tf.Variable(W)
    return W

def conv(X, n_out, activation_fn=tf.nn.tanh):
    raise NotImplemented
    filter_size = 3
    dilation_rate = 1
    n_in = X.get_shape().as_list()[-1]

    #return conv2d(X, n_out, filter_size, rate=dilation_rate, activation_fn=activation_fn)

    filter = tf.get_variable(name("conv"), [filter_size, filter_size, n_in, n_out], initializer=xavier_initializer())

    X = tf.nn.conv2d(X, filter, [1,1,1,1], 'SAME')

    if activation_fn:
        X = activation_fn(X)

    return X

def make_nn(input_shape, output_size, sizes):
    inputs_placeholder = tf.placeholder(tf.float32, [None] + list(input_shape))
    outputs_placeholder = tf.placeholder(tf.float32, [None, output_size])

    X = inputs_placeholder

    for size in sizes:
        X = conv2d(X, size, 3, activation_fn=tf.nn.relu)
        X = pool(X)

    X = flatten(X)
    
    X = linear(X, output_size, activation_fn=None)

    Y = X

    difference = Y - outputs_placeholder

    loss = tf.reduce_mean(tf.square(difference))

    train_op = tf.train.AdamOptimizer().minimize(loss)

    return inputs_placeholder, outputs_placeholder, train_op, loss, Y
