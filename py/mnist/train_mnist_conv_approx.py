import tensorflow as tf
import numpy as np

mnist = np.load("mnist.npy").item()

def pool(X):
    #return tf.nn.max_pool(X, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')
    return tf.nn.avg_pool(X, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')

def uppool(X):
    # allows changing sizes at runtime
    height = tf.shape(X)[1]
    width = tf.shape(X)[2]
    return tf.image.resize_images(X, (height*2, width*2))

def flatten(X):
    flat_size = np.prod(X.get_shape().as_list()[1:])
    return tf.reshape(X, [-1, flat_size])

def initializer(shape):
    n_in = np.product(shape[:-1])
    n_out = shape[-1]
    
    # xavier uniform init
    r = np.sqrt(6/(n_in + n_out))
    return tf.random_uniform(shape=shape, minval=-r, maxval=r)
    
    # xavier  random normal init
    #stddev = np.sqrt(3.0 / (n_in + n_out))
    #return tf.truncated_normal(shape=shape, stddev=stddev)

def conv2d(X, n_out, filter_size, activation_fn):
    n_in = X.get_shape().as_list()[-1]
    shape = (filter_size, filter_size, n_in, n_out)
    W = tf.Variable(initializer(shape))
    b = tf.Variable(tf.zeros(n_out))
    X = tf.nn.conv2d(X, W, [1,1,1,1], 'SAME')
    X = tf.nn.bias_add(X, b)
    if activation_fn:
        X = activation_fn(X)
    return X

def linear(X, n_out, activation_fn):
    n_in = X.get_shape().as_list()[-1]
    shape = (n_in, n_out)
    W = tf.Variable(initializer(shape))
    b = tf.Variable(tf.zeros(n_out))
    X = tf.matmul(X, W)
    X = tf.nn.bias_add(X, b)
    if activation_fn:
        X = activation_fn(X)
    return X

n_images, height, width = mnist["train_images"].shape
n_labels = 10
batch_size = 100
conv_sizes = [8, 16, 32]
n_hidden = 128

#activation = tf.nn.relu
#activation = None

x = np.linspace(-2, 2, 1024)
y = np.maximum(0.1*x, x)
c = np.polyfit(x, y, 4)
c0, c1, c2, c3, c4 = list(reversed(c))

def activation(X):
    X2 = X*X
    X3 = X*X2
    X4 = X2*X2
    return c0 + c1*X + c2*X2 + c3*X3 + c4*X4

if 0:
    import matplotlib.pyplot as plt
    import sys
    plt.plot(x, y, label="leaky relu")
    plt.plot(x, activation(x), label="linear approximation")
    plt.legend()
    plt.show()
    sys.exit(0)

def make_batch(images, labels, batch_size):
    indices = np.random.randint(len(images), size=batch_size)
    
    inputs = images[indices].reshape(batch_size, height, width, 1).astype(np.float32)/255.0
    outputs = np.eye(n_labels, dtype=np.float32)[labels[indices]]
    
    return inputs, outputs

inputs_placeholder = tf.placeholder(tf.float32, [None, height, width, 1])
outputs_placeholder = tf.placeholder(tf.float32, [None, n_labels])

X = inputs_placeholder

for size in conv_sizes:
    X = conv2d(X, size, 3, activation_fn=activation)
    X = pool(X)

X = flatten(X)

X = linear(X, n_hidden, activation_fn=activation)
X = linear(X, n_labels, activation_fn=None)

Y = X

difference = Y - outputs_placeholder

loss = tf.reduce_mean(tf.square(difference))

correct = tf.equal(
    tf.argmax(Y, axis=1),
    tf.argmax(outputs_placeholder, axis=1))

accuracy = tf.reduce_mean(tf.cast(correct, tf.float32))

train_op = tf.train.AdamOptimizer().minimize(loss)

sess = tf.InteractiveSession()
sess.run(tf.global_variables_initializer())

for iteration in range(4000 + 1):
    inputs, outputs = make_batch(mnist["train_images"], mnist["train_labels"], batch_size)

    _, train_accuracy = sess.run([train_op, accuracy], {inputs_placeholder: inputs, outputs_placeholder: outputs})
    
    if iteration % 100 == 0:
        inputs, outputs = make_batch(mnist["test_images"], mnist["test_labels"], 1000)
        test_accuracy = sess.run(accuracy, {inputs_placeholder: inputs, outputs_placeholder: outputs})
        print("%05d - %f / %f"%(iteration, train_accuracy, test_accuracy))

"""
should print something like:
00000 - 0.110000 / 0.110000
00100 - 0.970000 / 0.936000
00200 - 0.940000 / 0.961000
00300 - 0.960000 / 0.979000
00400 - 0.940000 / 0.976000
00500 - 0.990000 / 0.975000
...
"""

