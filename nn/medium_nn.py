import numpy as np

np.random.seed(1)
""""
l0 = np.array([
    [0,0],
    [0,1],
    [1,0],
    [1,1],
])
"""
expected_output = np.array([[0,1,1,0]]).T

m = 500
x0 = np.linspace(0, 2*np.pi, m).reshape(m, 1)
x1 = np.random.randn(*x0.shape)*2*np.pi

l0 = x0

def f(x):
    return np.sin(x)

expected_output0 = f(x0)
expected_output1 = f(x1)

syn0 = 2*np.random.random((1,10)) - 1
syn1 = 2*np.random.random((10,10)) - 1
syn2 = 2*np.random.random((10,1)) - 1

sigmoid = 1

if sigmoid:
    n = 1000
    rate = 0.5
    
    # sigmoid
    def activation(x):
        return 1/(1+np.exp(-x))

    def activation_derivative(x):
        return x*(1-x)
else:
    n = 10000
    rate = 0.01
    alpha = 0.1
    threshold = 1.0
    
    def activation(x):
        return np.where(x < 0, alpha*x, x)

    def activation_derivative(x):
        return np.where(x < 0, 0*x+alpha, 0*x+1)

ns = list(range(n))

errors = [[], [], []]
test_errors = [[], [], []]

for _ in ns:
    l1 = activation(np.dot(l0, syn0))
    l2 = activation(np.dot(l1, syn1))
    l3 = activation(np.dot(l2, syn2))
    #l4 = activation(np.dot(l3, syn3))

    if sigmoid:
        #l4_delta = (expected_output0-l4)*activation_derivative(l4)
        #l3_delta = l4_delta.dot(syn3.T)*activation_derivative(l3)
        l3_delta = (expected_output0-l3)*activation_derivative(l3)
        l2_delta = l3_delta.dot(syn2.T)*activation_derivative(l2)
        l1_delta = l2_delta.dot(syn1.T)*activation_derivative(l1)
    else:
        l4_delta = (expected_output0-l4)*activation_derivative(np.dot(l3, syn3))
        l4_delta = np.clip(l4_delta, -threshold, threshold)
        l3_delta = l4_delta.dot(syn3.T)*activation_derivative(np.dot(l2, syn2))
        l3_delta = np.clip(l3_delta, -threshold, threshold)
        l2_delta = l3_delta.dot(syn2.T)*activation_derivative(np.dot(l1, syn1))
        l2_delta = np.clip(l2_delta, -threshold, threshold)
        l1_delta = l2_delta.dot(syn1.T)*activation_derivative(np.dot(l0, syn0))
        l1_delta = np.clip(l1_delta, -threshold, threshold)
    
    #syn3 += rate*l3.T.dot(l4_delta)
    syn2 += rate*l2.T.dot(l3_delta)
    syn1 += rate*l1.T.dot(l2_delta)
    syn0 += rate*l0.T.dot(l1_delta)

    error = np.abs(expected_output0 - l3)
    errors[0].append(np.linalg.norm(error, 1))
    errors[1].append(np.linalg.norm(error, 2))
    errors[2].append(np.linalg.norm(error, np.inf))
    
    l1 = activation(np.dot(x1, syn0))
    l2 = activation(np.dot(l1, syn1))
    l3 = activation(np.dot(l2, syn2))
    #l4 = activation(np.dot(l3, syn3))

    error = np.abs(expected_output1 - l3)
    test_errors[0].append(np.linalg.norm(error, 1))
    test_errors[1].append(np.linalg.norm(error, 2))
    test_errors[2].append(np.linalg.norm(error, np.inf))

#print("output:")
#print(l3)
#print("error:")
#print(error)

from matplotlib import pyplot as plt
if 1:

    plt.plot(ns, errors[0], label=r'$L_1$')
    plt.plot(ns, errors[1], label=r'$L_2$')
    plt.plot(ns, errors[2], label=r'$L_\infty$')
    plt.plot(ns, test_errors[0], label=r'validation $L_1$')
    plt.plot(ns, test_errors[1], label=r'validatio $L_2$')
    plt.plot(ns, test_errors[2], label=r'validatio $L_\infty$')
    plt.legend()
    plt.show()
