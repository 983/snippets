import matplotlib.pyplot as plt
import sys
sys.path.append("C:/lib")
from point import Point
import random
import numpy as np
import collections
import itertools

random.seed(0)

class Segment(object):

    def __init__(self, a, b):
        self.a = a
        self.b = b

    def __iter__(self):
        yield self.a
        yield self.b
        raise StopIteration

    def __eq__(self, other):
        a, b = self
        return a == other.a and b == other.b
    
    def __ne__(self, other):
        a, b = self
        return a != other.a or b != other.b
    
    def other(self, p):
        a, b = self
        if a == p: return b
        if b == p: return a
        raise ValueError("%s must be endpoint of %s"%(str(p), str(self)))

    def endpoints(self):
        a, b = self
        return (a, b)

    def lerp(self, u=0.5):
        a, b = self
        return lerp(a, b, u)

    def split_at_points(self, points, segments):
        a, b = self
        ba = b - a
        ba2 = ba.dot(ba)
        def pseudo_dist_along_line(p):
            return ba.dot(p - a)
        points.sort(key=pseudo_dist_along_line)
        def on_line(p):
            d = pseudo_dist_along_line(p)
            return d > 0 and d < ba2
    
        points = list(filter(on_line, points))
        points.append(b)
        for b in points:
            segments.append(Segment(a, b))
            a = b

    def length_squared(self):
        a, b = self
        return a.dist_squared(b)

    def intersections_horizontal(self, p):
        a, b = self
    
        if p.y < min(a.y, b.y) or p.y >= max(a.y, b.y): return [[], [], []]

        if a.y == b.y: return [[], [], []]

        x = a.x + (p.y - a.y)*(a.x - b.x)/(a.y - b.y)
        
        q = Point(x, p.y)

        if x < p.x: return [[q], [], []]
        if x > p.x: return [[], [], [q]]
        return [[], [q], []]

    def __reversed__(self):
        b, a = self
        return Segment(a, b)

    def left_oriented(self):
        a, b = self
        if a.x < b.x: return self
        return Segment(b, a)

    def points(self, n):
        a, b = self
        return [lerp(a, b, 1.0/(n-1)*i) for i in range(n)]

    def set_endpoints(self, a, b):
        self.a = a
        self.b = b

    def tangent_vector(self, p):
        q = self.other(p)
        return q - p
    
    def __str__(self):
        a, b = self
        return "Seg(%s, %s)"%(str(a), str(b))
    
    __repr__ = __str__
    
def intersect(a, b, c, d, frac=False):
    if a == c or a == d or b == c or b == d:
        # ignore segments with same endpoints
        return []
    
    ba = b - a
    dc = d - c
    ca = c - a
    
    ba_det_dc = ba.det(dc)
    ca_det_dc = ca.det(dc)
    ca_det_ba = ca.det(ba)

    # parallel segments
    if ba_det_dc == 0:
        
        if ca_det_dc == 0:
            intersections = []
            
            ba2 = ba.dot(ba)
            dc2 = dc.dot(dc)
            
            if 0 <= ba.dot(c - a) <= ba2: intersections.append(c)
            if 0 <= ba.dot(d - a) <= ba2: intersections.append(d)
            if 0 <= dc.dot(a - c) <= dc2: intersections.append(a)
            if 0 <= dc.dot(b - c) <= dc2: intersections.append(b)
            
            return intersections
        else:
            # parallel, but not on same line
            return []

    if frac:
        t = Fraction(ca_det_ba, ba_det_dc)
        s = Fraction(ca_det_dc, ba_det_dc)
    else:
        t = ca_det_ba/ba_det_dc
        s = ca_det_dc/ba_det_dc
        
    if t >= 0 and t <= 1 and s >= 0 and s <= 1:
        return [a + s*ba]

    return []

class Graph(object):

    def __init__(self, vertices=None, edges=None):
        if vertices is None:
            vertices = set()
        
        if edges is None:
            edges = set()

        if not isinstance(vertices, set):
            vertices = set(vertices)
        
        if not isinstance(edges, set):
            edges = set(edges)
        
        self.vertices = vertices
        self.edges = edges

vertices = list(range(10))
edges = []
for _ in range(20):
    while True:
        a = random.choice(vertices)
        b = random.choice(vertices)

        if a == b: continue

        if (a, b) not in edges and (b, a) not in edges:
            edges.append((a, b))
            break

g = Graph(vertices, edges)

def random_point():
    x = random.random()*2 - 1
    y = random.random()*2 - 1
    return Point(x, y)

points = {v:random_point() for v in vertices}

def get_intersections(edges, points):
    intersections = []
    
    for i in range(len(edges)):
        a, b = edges[i]
        pa = points[a]
        pb = points[b]
        for j in range(i + 1, len(edges)):
            c, d = edges[j]
            pc = points[c]
            pd = points[d]

            # don't count endpoint intersections
            if a == c or a == d or b == c or b == d:
                continue

            s = intersect(pa, pb, pc, pd)

            intersections.extend(s)

    return intersections

def calculate_distances(vertices, edges):
    n = len(vertices)
    
    dist = np.full((n, n), np.inf)

    for a,b in edges:
        dist[a, b] = 1
        dist[b, a] = 1

    for k in range(n):
        for i in range(n):
            for j in range(n):
                dist[i, j] = min(dist[i, j], dist[i, k] + dist[k, j])
    
    return dist

d = calculate_distances(vertices, edges)

def subsets(values):
    for k in range(len(values)):
        yield from itertools.combinations(values, k)

for W in subsets(vertices):
    if len(W) == 0: continue

    dists = set()

    for v in vertices:
        if v in W: continue

        dists.add(tuple(d[W, v]))

    if len(dists) == len(vertices) - len(W):
        print("found separating set of size %d:"%len(W))
        print(W)
        break

best_points = None
smallest_n = 1000

def dist2_point_line(p, a, b):
    pa = p - a
    ba = b - a
    
    u = pa.dot(ba)/ba.dot(ba)

    if u < 0: u = 0
    if u > 1: u = 1

    q = a + u*ba

    return p.dist2(q)

def min_vertex_edge_dist2(edges, points):
    min_dist2 = np.inf
    for p, pp in points.items():
        for a,b in edges:
            if p == a or p == b: continue
            
            pa = points[a]
            pb = points[b]

            dist2 = dist2_point_line(pp, pa, pb)
            
            min_dist2 = min(min_dist2, dist2)
    
    return min_dist2

for _ in range(1000):
    temp_points = points.copy()

    for _ in range(3):
        v = random.choice(vertices)
        temp_points[v] = random_point()
    
    n = len(get_intersections(edges, temp_points))

    if smallest_n > n or (
        smallest_n == n and
        min_vertex_edge_dist2(edges, temp_points) >
        min_vertex_edge_dist2(edges, points)
    ):
        smallest_n = n
        points = temp_points

print("intersections:", smallest_n)

for intersection in get_intersections(edges, points):
    plt.plot([intersection.x],[intersection.y], 'x', lw=0.5, color='blue', ms=20)

for a,b in edges:
    a = points[a]
    b = points[b]
    plt.plot([a.x, b.x], [a.y, b.y], color='black')

for v in vertices:
    v = points[v]
    plt.plot([v.x],[v.y], 'o--', lw=0.5, color='white', ms=7, markeredgecolor='black')

plt.show()
