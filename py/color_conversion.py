import numpy as np
import matplotlib.pyplot as plt
from math import sqrt

def hue_to_rgb(h):
    assert(0.0 <= h <= 1.0)
    
    if h == 1.0:
        h = 0.0

    i = int(h*6)
    f = h*6.0 - i
    q = 1.0 - f
    
    if   i == 0: return (1, f, 0)
    elif i == 1: return (q, 1, 0)
    elif i == 2: return (0, 1, f)
    elif i == 3: return (0, q, 1)
    elif i == 4: return (f, 0, 1)
    else       : return (1, 0, q)

def srgb_to_xyz(rgb):
    r, g, b = rgb
    
    assert(0.0 <= r <= 1.0)
    assert(0.0 <= g <= 1.0)
    assert(0.0 <= b <= 1.0)
    
    a = 0.055

    x = r/12.92 if r <= 0.04045 else ((r + 0.055)/1.055)**2.4
    y = g/12.92 if g <= 0.04045 else ((g + 0.055)/1.055)**2.4
    z = b/12.92 if b <= 0.04045 else ((b + 0.055)/1.055)**2.4
    
    x2 = 0.4124564*x + 0.3575761*y + 0.1804375*z
    y2 = 0.2126729*x + 0.7151522*y + 0.0721750*z
    z2 = 0.0193339*x + 0.1191920*y + 0.9503041*z

    x2 *= 100.0
    y2 *= 100.0
    z2 *= 100.0
    
    return (x2, y2, z2)

def cuberoot(x):
    return x**(1.0/3.0)

def xyz_to_lab(xyz):
    x, y, z = xyz
    
    # D65 reference white point
    x /=  95.047
    y /= 100.000
    z /= 108.883

    x = 7.787*x + 4.0/29.0 if x <= 0.008856 else cuberoot(x)
    y = 7.787*y + 4.0/29.0 if y <= 0.008856 else cuberoot(y)
    z = 7.787*z + 4.0/29.0 if z <= 0.008856 else cuberoot(z)

    L = 116.0*y - 16.0
    a = 500.0*(x - y)
    b = 200.0*(y - z)

    return (L, a, b)

def delta_e_94(lab1, lab2):
    l1, a1, b1 = lab1
    l2, a2, b2 = lab2

    delta_L = l1 - l2
    C1 = sqrt(a1*a1 + b1*b1)
    C2 = sqrt(a2*a2 + b2*b2)
    delta_Cab = C1 - C2

    delta_a = a1 - a2
    delta_b = b1 - b2

    delta_Hab = sqrt(delta_a**2 + delta_b**2 - delta_Cab**2)

    sC = 1 + 0.045*C1
    sH = 1 + 0.015*C1

    delta_e_94 = sqrt(delta_L**2 + (delta_Cab/sC)**2 + (delta_Hab/sH)**2)
    
    return delta_e_94

def srgb_to_lab(rgb):
    return xyz_to_lab(srgb_to_xyz(rgb))

def hue_to_lab(hue):
    return srgb_to_lab(hue_to_rgb(hue))

n = 1000
hues = np.linspace(0, 1, n)
labs = [hue_to_lab(hue) for hue in hues]
deltas = [delta_e_94(labs[i], labs[i + 1]) for i in range(n - 1)]

deltas = np.array(deltas)
c = np.cumsum(deltas)
c /= np.max(c)

#plt.plot(hues[:-1], deltas, color='#000000')
for i in range(n - 2):
    ax = hues[i]
    bx = hues[i + 1]
    ay = deltas[i]
    by = deltas[i + 1]
    hue = hues[i]
    color = hue_to_rgb(hue)
    #plt.plot([ax, bx], [ay, by], color=color)

    ay = c[i]
    by = c[i + 1]
    plt.plot([ax, bx], [ay, by], color=color)
#plt.plot(hues[:-1], c, color='#666666')
plt.xlabel('hue')
plt.ylabel('normalized cumulative color difference')

hues = []
n_colors = 12
us = np.linspace(0, 1, n_colors, endpoint=False)
for u in us:
    d = u - c
    hue = np.argmin(d*d)/n
    hues.append(hue)

for u, hue in zip(us, hues):
    color = hue_to_rgb(hue)
    
    plt.plot([hue, hue], [0, u], color=color)
    plt.plot([0, hue], [u, u], color=color)
    plt.plot([0], [u], marker='o', color=color)
    plt.plot([hue], [0], marker='o', color=color)

plt.xlim([-0.1, 1.1])
plt.ylim([-0.1, 1.1])
plt.show()

n = len(hues)
#hues = np.linspace(0, 1, n, endpoint=False)
image = np.zeros((2*n, 2*n, 3))
for i in range(n):
    rgb = hue_to_rgb(hues[i])
    image[2*i, :] = rgb
    image[:, 2*i] = rgb
    r, g, b = rgb
    print("[%f, %f, %f],"%rgb)
plt.imshow(image, interpolation='nearest', vmin=0, vmax=1)
plt.axis('off')
plt.show()
