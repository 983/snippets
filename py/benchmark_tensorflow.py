import tensorflow as tf
import time

n = 1024
with tf.device('/gpu:0'):
    A = tf.ones((n, n))
    B = tf.ones((n, n))
    C = tf.matmul(A, B)

    with tf.Session() as sess:
        for _ in range(10):
            t = time.clock()
            sess.run(C)
            dt = time.clock() - t
            print(2e-9*n**3/dt, "gflops")
