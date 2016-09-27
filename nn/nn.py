import numpy as np

np.random.seed(1)

def sigmoid(x):
    return 1/(1 + np.exp(-x))

def sigmoid_derivative(x):
    return x * (1 - x)

layer0 = np.array([
    [0, 0, 1],
    [0, 1, 1],
    [1, 0, 1],
    [1, 1, 1],
])

expected_output = np.array([
    [0],
    [1],
    [1],
    [0],
])

n = 2000

error_l1   = np.zeros(n)
error_l2   = np.zeros(n)
error_linf = np.zeros(n)

def two_layers():
    weights0 = 2*np.random.random((3, 4)) - 1
    weights1 = 2*np.random.random((4, 1)) - 1

    for i in range(n):
        layer1 = sigmoid(layer0.dot(weights0))
        layer2 = sigmoid(layer1.dot(weights1))
        
        layer2_delta = (expected_output - layer2) * sigmoid_derivative(layer2)
        layer1_delta = layer2_delta.dot(weights1.T) * sigmoid_derivative(layer1)
        
        weights1 += layer1.T.dot(layer2_delta)
        weights0 += layer0.T.dot(layer1_delta)
        
        difference = np.abs(layer2_delta)
        error_l1  [i] = np.sum(difference)
        error_l2  [i] = np.sum(difference*difference)
        error_linf[i] = np.max(difference)

    print(layer2)
    
def three_layers():
    weights0 = 2*np.random.random((3, 4)) - 1
    weights1 = 2*np.random.random((4, 3)) - 1
    weights2 = 2*np.random.random((3, 1)) - 1

    for i in range(n):
        layer1 = sigmoid(layer0.dot(weights0))
        layer2 = sigmoid(layer1.dot(weights1))
        layer3 = sigmoid(layer2.dot(weights2))
        
        layer3_delta = (expected_output -   layer3) * sigmoid_derivative(layer3)
        layer2_delta = layer3_delta.dot(weights2.T) * sigmoid_derivative(layer2)
        layer1_delta = layer2_delta.dot(weights1.T) * sigmoid_derivative(layer1)

        weights2 += layer2.T.dot(layer3_delta)
        weights1 += layer1.T.dot(layer2_delta)
        weights0 += layer0.T.dot(layer1_delta)

        difference = np.abs(layer3_delta)
        error_l1  [i] = np.sum(difference)
        error_l2  [i] = np.sum(difference*difference)
        error_linf[i] = np.max(difference)

    print(layer3)
    
def four_layers():
    weights0 = 2*np.random.random((3, 4)) - 1
    weights1 = 2*np.random.random((4, 3)) - 1
    weights2 = 2*np.random.random((3, 3)) - 1
    weights3 = 2*np.random.random((3, 1)) - 1

    for i in range(n):
        layer1 = sigmoid(layer0.dot(weights0))
        layer2 = sigmoid(layer1.dot(weights1))
        layer3 = sigmoid(layer2.dot(weights2))
        layer4 = sigmoid(layer3.dot(weights3))
        
        layer4_delta = (expected_output -   layer4) * sigmoid_derivative(layer4)
        layer3_delta = layer4_delta.dot(weights3.T) * sigmoid_derivative(layer3)
        layer2_delta = layer3_delta.dot(weights2.T) * sigmoid_derivative(layer2)
        layer1_delta = layer2_delta.dot(weights1.T) * sigmoid_derivative(layer1)

        weights3 += layer3.T.dot(layer4_delta)
        weights2 += layer2.T.dot(layer3_delta)
        weights1 += layer1.T.dot(layer2_delta)
        weights0 += layer0.T.dot(layer1_delta)

        difference = np.abs(layer4_delta)
        error_l1  [i] = np.sum(difference)
        error_l2  [i] = np.sum(difference*difference)
        error_linf[i] = np.max(difference)

    print(layer4)

#two_layers()
three_layers()
#four_layers()
    
from matplotlib import pyplot as plt
plt.plot(np.linspace(1, n, n), error_l1)
plt.plot(np.linspace(1, n, n), error_l2)
plt.plot(np.linspace(1, n, n), error_linf)
plt.show()
