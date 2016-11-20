import numpy as np
from matplotlib import pyplot as plt
from nn import *
import mnist

data_length = 28*28
n_labels = 10
hidden_dim = 100
batch_size = 100

np.random.seed(0)

INPUT = Constant()
OUTPUT = Constant()

Y = INPUT
Y = LinearLayer(Y, (data_length, hidden_dim))
Y = Relu(Y)
Y = LinearLayer(Y, (hidden_dim, hidden_dim))
Y = Relu(Y)
Y = LinearLayer(Y, (hidden_dim, hidden_dim))
Y = Relu(Y)
Y = LinearLayer(Y, (hidden_dim, n_labels))
L = SoftmaxCrossEntropyLoss(Y, OUTPUT)

accuracies = []
for epoch in range(100*100):
    batch_idx = np.random.randint(len(mnist.TRAIN_DATA), size=batch_size)
    
    INPUT.X = mnist.TRAIN_DATA[batch_idx, :]
    OUTPUT.X = mnist.TRAIN_LABELS[batch_idx, :]
    
    L.forward()
    L.backward()
    
    if epoch % 100 == 0:
        INPUT.X = mnist.TEST_DATA
        labels_guess = np.argmax(Y.forward(), 1)
        labels_true = np.argmax(mnist.TEST_LABELS, 1)
        accuracy = np.mean(labels_guess == labels_true)
        accuracies.append(accuracy)
        print("%6d: accuracy: %f"%(epoch, accuracy))

plt.plot(accuracies[40:])
plt.show()
