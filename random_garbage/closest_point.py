import numpy as np
from matplotlib import pyplot as plt

def gss(f, x0, x1):
    for _ in range(20):
        
        s = 0.61803398874989479 * (x1 - x0)
        new_x0 = x1 - s
        new_x1 = x0 + s

        x0_smaller = f(new_x0) < f(new_x1)
        x0 = x0 if x0_smaller else new_x0
        x1 = new_x1 if x0_smaller else x1
        
    return 0.5*(x0 + x1)

n = 1000
x0 = -5
x1 = +5
ts = np.linspace(x0, x1, n)

def f(x):
    #return x*x*x
    return np.cos(x*10.0) + 10.0*np.exp(-x*x) - x

def dist2(t, x, y):
    dx = t - x
    dy = f(t) - y
    return dx*dx + dy*dy

def df(x):
    return 3.0*x*x

def ddt_dist2(t, x, y):
    ##return 2.0*(t - x) + df(t)*2.0*(f(t) - y)
    eps = 1e-5
    return 0.5/eps*(dist2(t + eps, x, y) - dist2(t - eps, x, y))

def foo(t):
    return dist2(t, x, y)

x = 2
y = 3
print("start:")
print(x)

i = np.argmin(dist2(ts, x, y))
t = ts[i]
print("approximate minimum:")
print(t, foo(t))

t = x
for _ in range(50):
    t -= 0.01*ddt_dist2(t, x, y)

print("gradiend descend:")
print(t, foo(t))

t = x
for _ in range(50):
    t -= dist2(t, x, y)/ddt_dist2(t, x, y)
    
print("newton's method:")
print(t, foo(t))

m = 20
ts2 = np.linspace(x0, x1, m + 1)
i = np.argmin(foo(ts2))
t = ts2[i]
dx = 1.0/m * (x1 - x0)
x0 = t - dx
x1 = t + dx
t = gss(foo, x0, x1)

print("interval golden ratio search:")
print(t, foo(t))

minima = []
for a, b, c in zip(ts, ts[1:], ts[2:]):
    fa, fb, fc = foo(a), foo(b), foo(c)
    if fb < fa and fb < fc:
        minima.append(a)

minima = np.array(minima)

plt.plot(ts, foo(ts))
plt.plot(ts, f(ts))
plt.plot(ts2, foo(ts2))
plt.plot(minima, foo(minima), 'ro')
plt.show()
