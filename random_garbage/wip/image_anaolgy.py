import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np
import scipy.misc
import scipy.signal
import time
import os
import cv2

def flatten(x):
    return np.reshape(x, np.product(x.shape))

def make_grid(images, ny, nx):
    n, height, width = images.shape
    assert(nx*ny == n)
    images = images.reshape((ny, nx, height, width))
    images = np.concatenate(images, axis=1)
    images = np.concatenate(images, axis=1)
    return images

import numba
from numba import jit, float32, int32

@jit(nopython=True, nogil=True)
def clamp(x, a, b):
    if x < a: return a
    if x > b: return b
    return x

@jit(nopython=True, nogil=True)
def calculate_cost(A, B, iA, jA, iB, jB, radius):
    A_height, A_width, A_depth = A.shape
    B_height, B_width, B_depth = B.shape

    assert(A_depth == B_depth)
    
    r = np.arange(-radius, radius + 1)
    
    cost = 0.0
    for di in r:
        for dj in r:
            i0 = clamp(iA + di, 0, A_height - 1)
            j0 = clamp(jA + dj, 0, A_width  - 1)
            
            i1 = clamp(iB + di, 0, B_height - 1)
            j1 = clamp(jB + dj, 0, B_width  - 1)
            
            for k in range(A_depth):
                delta = A[i0, j0, k] - B[i1, j1, k]
                cost += delta*delta
    
    return np.sqrt(cost)

@jit(nopython=True, nogil=True)
def improve(costs, iAB, jAB, A, B, iA, jA, iB, jB, radius):

    cost = calculate_cost(A, B, iA, jA, iB, jB, radius)

    if cost < costs[iA, jA]:
        costs[iA, jA] = cost
        iAB[iA, jA] = iB
        jAB[iA, jA] = jB

@jit(nopython=True, nogil=True)
def minimize_cost(A, B, radius):
    A_height, A_width, A_depth = A.shape
    B_height, B_width, B_depth = B.shape

    # indices from A into closest patch in B
    iAB = np.random.randint(0, B_height, (A_height, A_width))
    jAB = np.random.randint(0, B_width , (A_height, A_width))

    costs = 1e10*np.ones((A_height, A_width), dtype=np.float32)

    for iteration in range(5):
        print("minimize_cost iteration", iteration)
        
        iBs = np.random.randint(0, B_height, (A_height, A_width))
        jBs = np.random.randint(0, B_width , (A_height, A_width))

        for iA in range(A_height):

            for jA in range(A_width):
                iB = iBs[iA, jA]
                jB = jBs[iA, jA]

                improve(costs, iAB, jAB, A, B, iA, jA, iB, jB, radius)

        r = np.arange(-1, 2)
        
        for iA in range(A_height):
            for jA in range(A_width):
                for di in r:
                    for dj in r:
                        iNeighbor = clamp(iA + di, 0, A_height - 1)
                        jNeighbor = clamp(jA + dj, 0, A_width  - 1)

                        iB = clamp(iAB[iNeighbor, jNeighbor] - di, 0, B_height - 1)
                        jB = clamp(jAB[iNeighbor, jNeighbor] - dj, 0, B_width  - 1)

                        improve(costs, iAB, jAB, A, B, iA, jA, iB, jB, radius)
        
    return iAB, jAB, costs

def image_to_vgg(image):
    assert(image.dtype == np.uint8)
    image = image[:, :, :3]
    image = scipy.misc.imresize(image, (224, 224))
    image = image.astype(np.float32)

    r = image[:, :, 0] - 103.939
    g = image[:, :, 1] - 116.779
    b = image[:, :, 2] - 123.68

    bgr_image = np.stack([b, g, r], axis=2)

    return bgr_image

def load_image_vgg(path):
    return image_to_vgg(scipy.misc.imread(path))

image1 = load_image_vgg("avatar.png")
image2 = load_image_vgg("monalisa.png")

def make_pyramid(image, n):
    differences = []
    for i in range(n):
        smaller = cv2.pyrDown(image)
        larger = cv2.pyrUp(smaller)
        difference = image - larger
        differences.append(difference)
        image = smaller
    return image, differences

def reconstruct_pyramid(image, differences):
    for i in reversed(range(len(differences))):
        image = cv2.pyrUp(image) + differences[i]
    return image

if 0:
    n = 5
    image1, differences1 = make_pyramid(image1, n=n)
    image2, differences2 = make_pyramid(image2, n=n)

    image = image1
    for i in reversed(range(n)):
        d1 = differences1[i]
        d2 = differences2[i]
        
        iAB, jAB, costs = minimize_cost(d1, d2, 1)

        difference = d1.copy()
        difference[iAB, jAB] = d2

        image = cv2.pyrUp(image) + difference

    plt.imshow(image[:, :, 0], cmap='gray')
    plt.show()

kernel = np.array([[1, 2, 4, 2, 1]], dtype=np.float32)
kernel = kernel.T@kernel
kernel = 1.0/np.sum(kernel) * kernel
# height, width, n_in, n_out
kernels = kernel.reshape((5, 5, 1, 1))

height = 224
width  = 224

input_placeholder = tf.placeholder(tf.float32, shape=(None, height, width, 1))
kernels = tf.constant(kernels, name="filters")

X = input_placeholder
X = tf.nn.conv2d(X, kernels, [1,1,1,1], padding='SAME')
Y = X

image = scipy.misc.imread("avatar.png", flatten=True)/256.0

sess = tf.InteractiveSession()

images = image.reshape((1, height, width, 1))
feed_dict = {
    input_placeholder:images,
}
results = sess.run(Y, feed_dict=feed_dict)

result = results[0].reshape((height, width))

deconvolved = tf.Variable(tf.random_uniform((1, height, width, 1)))
deconvolved = tf.Variable(tf.fill((1, height, width, 1), np.mean(results)))
deconvolved = tf.Variable(results)
convolved = tf.nn.conv2d(deconvolved, kernels, [1,1,1,1], padding='SAME')
difference = Y - convolved
loss = tf.reduce_mean(difference*difference)

optimizer = tf.train.AdamOptimizer()
#optimizer = tf.train.GradientDescentOptimizer(1e4)
reduce_loss = optimizer.minimize(loss)

init = tf.global_variables_initializer()
sess.run(init)

for i in range(1000):
    de, _, lo = sess.run([deconvolved, reduce_loss, loss], feed_dict=feed_dict)
    if i % 100 == 0:
        print(lo)

de = de.reshape((height, width))

plt.subplot(1, 3, 1)
plt.imshow(image, cmap='gray')
plt.subplot(1, 3, 2)
plt.imshow(result, cmap='gray')
plt.subplot(1, 3, 3)
plt.imshow(de, cmap='gray')
plt.show()

"""
with open("synset.txt", "r") as f:
    label_names = f.read().strip().split("\n")

print("loaded label names")

d = np.load("vgg19.npy", encoding='latin1').item()

print("loaded weights")

relus = []

def conv_layer(X, name):
    with tf.variable_scope(name):
        filters = tf.constant(d[name][0], name="filters")
        biases = tf.constant(d[name][1], name="biases")
        
        X = tf.nn.conv2d(X, filters, [1,1,1,1], padding='SAME')
        X = tf.nn.bias_add(X, biases)
        X = tf.nn.relu(X)

        relus.append(X)
        
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
batch = [load_image_vgg(path) for path in paths]

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

if 1:
    batch = [
        load_image_vgg("avatar.png"),
        load_image_vgg("monalisa.png"),
    ]

    results = sess.run(relus[4], feed_dict={input_placeholder:batch})

    ny = 4
    nx = 4
    filtered_images = results[0].transpose([2, 0, 1])
    filtered_images = filtered_images[:ny*nx]
    image = make_grid(filtered_images, ny, nx)
    plt.subplot(1, 2, 1)
    plt.imshow(image, cmap='gray')
    plt.axis('off')
    plt.subplot(1, 2, 2)
    plt.imshow(filtered_images[0], cmap='gray')
    plt.axis('off')
    plt.show()


    
"""
