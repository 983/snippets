import numpy as np
from numpy.linalg import *
from matplotlib import pyplot as plt

def check(f, f_approx, a, b):
    n = 10000
    x = np.linspace(a, b, n)
    y = f(x)
    y_approx = f_approx(x)
    err = np.abs(y - y_approx)
    print("max_err:", np.max(err))
    plt.plot(x, err, color="red")
    plt.show()

def fit(f, a, b, degree = 10):
    
    n = 1000
    u = np.cos(np.linspace(0, np.pi, n))*0.5 + 0.5
    x = a + u*(b - a)
    y = f(x)

    """
    TODO does not handle odd degrees
    
    if "every odd coefficient is zero":
        x = np.reshape(x, (n, 1))
        cols = [x**i for i in reversed(range(0, degree, 2))]
        X = np.hstack(cols)
        c = np.zeros(degree)
        c[1::2] = lstsq(X, y)[0]
    
    if "every even coefficient is zero":
        x = np.reshape(x, (n, 1))
        cols = [x**i for i in reversed(range(1, degree + 1, 2))]
        X = np.hstack(cols)
        c = np.zeros(degree)
        c[::2] = lstsq(X, y)[0]
    else:
    """
    c = np.polyfit(x, f(x), degree)

    print("coeffs:", ", ".join(map(str, c)))
    
    check(f, np.poly1d(c), a, b)

fit(np.arctan, -1, +1)
fit(np.cos, -np.pi/2, +np.pi/2)
fit(np.sin, -np.pi/2, +np.pi/2)
