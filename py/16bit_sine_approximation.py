import numpy as np
import matplotlib.pyplot as plt

a = 30000
b = 0

y = [a]
temp = [b]

for _ in range(402):
    a += b >> 6
    b -= a >> 6
    
    y.append(a)
    
y = np.float64(y)
y /= y.max()

t = np.linspace(0, 2*np.pi, len(y), endpoint=False)
y2 = np.cos(t)
    
err = np.linalg.norm(y - y2)

print(err)

plt.plot(y)
plt.plot(y2)
plt.show()
