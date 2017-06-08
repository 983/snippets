import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np
import scipy.misc
import scipy.spatial
import time
import os

import numba
from numba import jit, vectorize, guvectorize, float32, int32

@jit(nopython=True, nogil=True)
def calculate_cost(A, B, iA, jA, iB, jB):
    A_height, A_width, A_depth = A.shape
    B_height, B_width, B_depth = B.shape
    
    r = np.arange(-1, 2)
    
    cost = 0.0
    for di in r:
        for dj in r:
            i0 = clamp(iA + di, 0, A_height - 1)
            j0 = clamp(jA + dj, 0, A_width  - 1)
            
            i1 = clamp(iB + di, 0, B_height - 1)
            j1 = clamp(jB + dj, 0, B_width  - 1)
            
            for k in range(3):
                delta = A[i0, j0, k] - B[i1, j1, k]
                cost += delta*delta
    
    return np.sqrt(cost)

@jit(nopython=True, nogil=True)
def improve(costs, iAB, jAB, A, B, iA, jA, iB, jB):

    cost = calculate_cost(A, B, iA, jA, iB, jB)

    if cost < costs[iA, jA]:
        costs[iA, jA] = cost
        iAB[iA, jA] = iB
        jAB[iA, jA] = jB

@jit(nopython=True, nogil=True)
def clamp(x, a, b):
    if x < a: return a
    if x > b: return b
    return x

@jit(nopython=True, nogil=True)
def minimize_cost(A, B):
    A_height, A_width, A_depth = A.shape
    B_height, B_width, B_depth = B.shape

    # indices from A into closest patch in B
    iAB = np.random.randint(0, B_height, (A_height, A_width))
    jAB = np.random.randint(0, B_width , (A_height, A_width))

    costs = 1e10*np.ones((A_height, A_width), dtype=np.float32)

    for iteration in range(5):
        print(iteration)
        
        iBs = np.random.randint(0, B_height, (A_height, A_width))
        jBs = np.random.randint(0, B_width , (A_height, A_width))

        for iA in range(A_height):

            for jA in range(A_width):
                iB = iBs[iA, jA]
                jB = jBs[iA, jA]

                improve(costs, iAB, jAB, A, B, iA, jA, iB, jB)

        r = np.arange(-1, 2)
        
        for iA in range(A_height):
            for jA in range(A_width):
                for di in r:
                    for dj in r:
                        iNeighbor = clamp(iA + di, 0, A_height - 1)
                        jNeighbor = clamp(jA + dj, 0, A_width  - 1)

                        iB = clamp(iAB[iNeighbor, jNeighbor] - di, 0, B_height - 1)
                        jB = clamp(jAB[iNeighbor, jNeighbor] - dj, 0, B_width  - 1)

                        improve(costs, iAB, jAB, A, B, iA, jA, iB, jB)
        
    return costs

def flatten(x):
    return np.reshape(x, np.product(x.shape))

def knnsearch(dataset, testset, k):
    kdtree = scipy.spatial.cKDTree(dataset, leafsize=10)
    distances, indices = kdtree.query(testset, k, eps=2.0, n_jobs=4)
    return (distances, indices)

def load_image(path):
    image = scipy.misc.imread(path)[:, :, :3]
    image = scipy.misc.imresize(image, (224, 224))
    image = image.astype(np.float32)

    r = image[:, :, 0] - 103.939
    g = image[:, :, 1] - 116.779
    b = image[:, :, 2] - 123.68

    bgr_image = np.stack([b, g, r], axis=2)

    return bgr_image

image1 = load_image("avatar.png")
image2 = load_image("monalisa.png")

def make_windows(image, r):
    # make (2*r+1, 2*r+1)-shaped windows with clamped boundary conditions
    height, width = image.shape[:2]
    i = np.arange(height)
    j = np.arange(width)
    j, i = np.meshgrid(j, i)

    return np.transpose([
        flatten(image[
            np.clip(i + di, 0, height - 1),
            np.clip(j + dj, 0, width - 1)])
        for di in range(-r, r+1)
        for dj in range(-r, r+1)
    ])

image1 = np.mean(image1, axis=2)
image2 = np.mean(image2, axis=2)
r = 3
windows1 = make_windows(image1, r=r)
windows2 = make_windows(image2, r=r)

windows1 /= np.mean(windows1, axis=1).reshape((len(windows1), 1))
windows2 /= np.mean(windows2, axis=1).reshape((len(windows2), 1))

t = time.clock()
distances, indices = knnsearch(windows1, windows2, k=1)
dt = time.clock() - t
print("%f seconds"%dt)

if 1:
    print(indices[0])
    print(np.argmin(np.sum((windows1 - windows2[0].reshape(1, (2*r+1)**2))**2, axis=1)))

temp = flatten(image2.copy())
temp[indices] = flatten(image1)
temp = temp.reshape((224, 224))
plt.subplot(1, 2, 1)
plt.imshow(temp, cmap='gray')
plt.subplot(1, 2, 2)
plt.imshow(image2, cmap='gray')
plt.show()

"""
plt.imshow(image1[:, :, 0], cmap='gray')
plt.show()
"""
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

def make_grid(images, ny, nx):
    n, height, width = images.shape
    assert(nx*ny == n)
    images = images.reshape((ny, nx, height, width))
    images = np.concatenate(images, axis=1)
    images = np.concatenate(images, axis=1)
    return images

if 1:
    batch = [
        load_image("avatar.png"),
        load_image("monalisa.png"),
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
