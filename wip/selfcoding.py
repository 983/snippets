import matplotlib.pyplot as plt
import numpy as np
import itertools
import time

def draw_circle(x, y, radius):
    circle = plt.Circle((x, y), radius, fc='white', ec='black', fill=True, zorder=10)
    plt.gcf().gca().add_artist(circle)

def draw_line(ax, ay, bx, by, color='black'):
    plt.plot([ax, bx], [ay, by], color=color)

p = [
    [4, 1],
    [0, 1],
    [1, 2],
    [3, 2],
    [1, 0],
    [3, 0],
]

n = len(p)

V = list(range(n))

costs = {
    (1, 2): 18,
    (1, 4):  6,
    (4, 5): 25,
    (3, 5): 38,
    (2, 3): 20,
    (0, 3):  4,
    (0, 5): 40,
    (3, 4):  5,
    (2, 4): 10,
}

m = [
    [9, -1, -1, -1, -1, -1],
    [5, 20, 2, 5, 5, 5],
    [4, 8, 10, 7, 6, 5],
    [13, 14, 10, 15, 12, 11],
    [6, 8, 10, 9, 11, 7],
    [15, 16, 3, 18, 17, 19],
]

m2 = [
    [9, -1, -1, -1, -1, -1],
    [5, 20, 2, 5, 5, 5],
    [4, 8, 10, 7, 6, 5],
    [13, 14, 10, 15, 12, 11],
    [6, 8, 10, 9, 11, 7],
    [15, 16, 3, 18, 17, 19],
]

t = time.clock()
count = 0
for helpers in itertools.product(V, repeat=n):
    count += 1
dt = time.clock()

print(count, dt)


t = time.clock()
count = 0
for helpers in itertools.product([0, 1], repeat=len(costs)):
    count += 1
dt = time.clock()

print(count, dt)

"""
for (a,b), c in costs.items():
    ax, ay = p[a]
    bx, by = p[b]
    mx = (ax + bx)*0.5
    my = (ay + by)*0.5
    draw_line(ax, ay, bx, by)
    plt.text(mx, my, str(c), ha='center', va='center', bbox={'boxstyle':'round', 'facecolor':'white', 'pad':0.5})

for i, (x,y) in enumerate(p):
    draw_circle(x, y, 0.11)
    #draw_circle(x, y, 0.1, 'white')
    plt.text(x, y, str(i), zorder=11, ha='center', va='center')

plt.axes().set_aspect('equal', 'datalim')
plt.grid(True)
plt.show()
"""
