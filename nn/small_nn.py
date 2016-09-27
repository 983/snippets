import numpy as np
import numpy.linalg

np.random.seed(1)

def activation(x):
    return 1/(1 + np.exp(-x))

def activation_derivative(x):
    return activation(x)*(1 - activation(x))

def activation_derivative2(x):
    return x*(1-x)

def f(x):
    return np.exp(-x*x)
    #return activation(0.42*activation(0.1337*x))

m = 10
r = 1
x0 = 2*r*np.random.rand(m, 1) - r
x1 = 2*r*np.random.rand(m, 1) - r
y = f(x0)
syn0 = 2*np.random.rand(1, 10) - 1
syn1 = 2*np.random.rand(10, 1) - 1

err0 = []
err1 = []

n = 12345
rate = 0.1

ns = list(range(n))

for j in ns:
    l1 = np.dot(x0,syn0)
    al1 = activation(l1)
    l2 = np.dot(al1,syn1)
    al2 = activation(l2)
    
    #l2_delta =            (y - al2)*activation_derivative2(al2)
    #l1_delta = l2_delta.dot(syn1.T)*activation_derivative2(al1)
    l2_delta =            (y - al2)*activation_derivative(l2)
    l1_delta = l2_delta.dot(syn1.T)*activation_derivative(l1)
    
    syn1 += rate*al1.T.dot(l2_delta)
    syn0 += rate*x0.T.dot(l1_delta)

    err0.append(np.linalg.norm(np.abs(f(x0) - al2), 1))
    
    l1 = activation(np.dot(x1,syn0))
    l2 = activation(np.dot(l1,syn1))
    
    err1.append(np.linalg.norm(np.abs(f(x1) - l2), 1))

print(l2)
print(l2 - y)
print(syn0)
print(syn1)

from matplotlib import pyplot as plt

plt.plot(ns, err0, label="error")
plt.plot(ns, err1, label="validation error")
plt.legend()
plt.show()
