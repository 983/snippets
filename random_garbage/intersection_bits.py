import matplotlib.pyplot as plt
from fractions import Fraction
import itertools
import random

max_bits = 0
def check(x):
    global max_bits
    n = x.bit_length()
    if n > max_bits:
        max_bits = n
        print(max_bits, xy)

def intersect(xy):
    ax, ay, bx, by, cx, cy, dx, dy = xy

    bax = bx - ax
    bay = by - ay
    dcx = dx - cx
    dcy = dy - cy

    cax = cx - ax
    cay = cy - ay

    denominator = dcx*bay - dcy*bax

    if denominator == 0:
        return

    #check(ax*denominator + (dcx*cay - dcy*cax)*bax)
    #check(ax*denominator + (dcx*cay - dcy*cax)*bax)
    
    u = Fraction(dcx*cay - dcy*cax, denominator)
    v = Fraction(bax*cay - bay*cax, denominator)

    # segments intersect if u and v in interval [0, 1]

    sx = ax + u*bax
    sy = ay + u*bay
    tx = cx + v*dcx
    ty = cy + v*dcy

    assert(sx == tx)
    assert(sy == ty)

    check(sx.numerator)
    check(sy.numerator)
    check(tx.numerator)
    check(ty.numerator)
    
    """
    plt.plot([tx], [ty], 'ro')

    plt.plot([ax, bx], [ay, by])
    plt.plot([cx, dx], [cy, dy])
    plt.show()
    """
"""
n, 3*n + 1 for n > 2
1, 2
2, 6
3, 10
4, 13
5, 16
6, 19
7, 22
8, 25
9, 28
10, 31
11, 34
12, 37
13, 40
14, 43
15, 46
16, 49
17, 52
18, 55
19, 58
"""

random.seed(1337)
for i in range(1000*1000*1000):
    n = 10
    xy = [random.getrandbits(n) for _ in range(8)]
    intersect(xy)
"""
for n in range(10, 20):
    max_bits = 0
    r = 2**n - 1
    for xy in itertools.product([-r, -1, 0, +1, r], repeat=8):
        intersect(xy)
"""
