import numpy as np
from matplotlib import pyplot as plt
from nn import *
import mnist

train_images, train_labels = mnist.load_train()
test_images, test_labels = mnist.load_test()

data_length = 28*28
hidden_dim = 123
n_labels = 10

np.random.seed(0)
CX = Constant(None)
CY = Constant(None)
Y = CX
Y = Sigmoid(Add(Mul(Y, Weight(data_length, hidden_dim)), Weight(1, hidden_dim)))
Y = Sigmoid(Add(Mul(Y, Weight(hidden_dim, n_labels)), Weight(1, n_labels)))
L = Loss(Y, CY)

CX.X = test_images
CY.X = test_labels
print(L.forward())
print("max:", np.max(Y.forward()))
# Warning: Does not work yet.
# RuntimeWarning: overflow encountered in exp
# TODO investigate effects of loss function (try that log thing)
# and try relu instead if Sigmoid
L.backward()
print(L.forward())
print("max:", np.max(Y.forward()))
