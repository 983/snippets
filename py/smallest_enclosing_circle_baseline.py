import random, time, math, sys
random.seed(0)

# only required for plotting
import matplotlib.pyplot as plt
plt.axes().set_aspect('equal', 'datalim')

def draw_circle(cx, cy, r, **kwargs):
    x = []
    y = []
    # actually one more point
    # the first point appears twice to close circle
    n_points = 128
    delta_angle = 2.0*math.pi/n_points
    for i in range(n_points):
        angle = delta_angle*i
        x.append(cx + r*math.cos(angle))
        y.append(cy + r*math.sin(angle))
    # close circle
    x.append(x[0])
    y.append(y[0])
    plt.plot(x, y, **kwargs)

class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other):
        return Point(self.x - other.x, self.y - other.y)

    def __rmul__(self, other):
        return Point(self.x*other, self.y*other)

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def dot(self, other):
        return self.x*other.x + self.y*other.y

    def det(self, other):
        return self.x*other.y - self.y*other.x

    def left(self):
        # vector left-perpendicular to current point interpreted as a vector
        return Point(-self.y, self.x)

    def len2(self):
        x = self.x
        y = self.y
        return x*x + y*y

    def dist2(self, other):
        dx = self.x - other.x
        dy = self.y - other.y
        return dx*dx + dy*dy
        
    def len(self):
        x = self.x
        y = self.y
        return np.sqrt(x*x + y*y)

    def dist(self, other):
        dx = self.x - other.x
        dy = self.y - other.y
        return math.sqrt(dx*dx + dy*dy)
        
    def __str__(self):
        return "Point(%s, %s)"%(str(self.x), str(self.y))

    def __hash__(self):
        # TODO evaluate mixing strategies
        return hash(self.x) ^ hash(self.y)

def line_point_test(a, b, p):
    # > 0:  left of line
    # = 0:       on line
    # < 0: right of line
    
    return (b - a).det(p - a)

class CircleFromThreePoints(object):
    def __init__(self, p0, p1, p2):
        self.points = [p0, p1, p2]

        a = p0.x
        b = p0.y
        
        e = p1.x
        f = p1.y
        
        i = p2.x
        j = p2.y
        
        c = a*a + b*b
        g = e*e + f*f
        k = i*i + j*j

        u = e*j - i*f
        v = b*i - a*j
        w = a*f - b*e

        t = c*u + g*v + k*w
        
        s = u + v + w

        if s == 0:
            raise ValueError("Tried to create circle %s from colinear points"%str(self))

        q = c*(j - f) + g*(b - j) + k*(f - b)
        r = c*(e - i) + g*(i - a) + k*(a - e)

        if s < 0:
            q = -q
            r = -r
            s = -s
            t = -t

        #print("approximate radius:", np.sqrt((a + q*0.5/s)**2 + (b + r*0.5/s)**2))

        p = (2*s*a + q)**2 + (2*s*b + r)**2

        self.precomputed = [q, r, s, t, p]

    def compare_radius(self, other):
        p0 = self.precomputed[4]
        s0 = self.precomputed[2]
        p1 = other.precomputed[4]
        s1 = other.precomputed[2]
        
        # > 0: self.radius > other.radius
        # = 0: self.radius = other.radius
        # < 0: self.radius < other.radius
        return p0*s1*s1 - p1*s0*s0

    def contains(self, p):
        q,r,s,t = self.precomputed[:4]
        
        m = p.x
        n = p.y
        o = m*m + n*n

        result = t - m*q - n*r - o*s

        if 0:
            a,b,c = self.points

            if line_point_test(a, b, c) < 0:
                c,a = a,c
        
            result2 = np.linalg.det([
                [a.x, a.y, a.len2(), 1],
                [b.x, b.y, b.len2(), 1],
                [c.x, c.y, c.len2(), 1],
                [p.x, p.y, p.len2(), 1],
            ])

            assert(np.abs(result - result2) < 1e-5)

        # > 0: point in circle
        # = 0: point on circle
        # < 0: point not in circle
        return result >= 0

    def draw(self, **kwargs):
        center = self.approximate_center()
        radius = self.approximate_radius()

        draw_circle(center.x, center.y, radius, **kwargs)

    def __str__(self):
        a,b,c = self.points
        return "Circle(%s, %s, %s)"%(str(a), str(b), str(c))

    def approximate_center(self):
        a,b,c = self.points

        denominator = a.det(b) + b.det(c) + c.det(a)

        if denominator == 0:
            raise ValueError("Degenerate circle %s created from colinear points"%str(self))
        
        p = a.dot(a)*(c - b) + b.dot(b)*(a - c) + c.dot(c)*(b - a)

        # loss of precision in division
        center = 0.5/denominator * p.left()

        return center

    def approximate_radius(self):
        # loss of precision in sqrt
        return self.points[0].dist(self.approximate_center())

class CircleFromTwoPoints(object):
    def __init__(self, a, b):
        self.a = a
        self.b = b

    def approximate_center(self):
        return 0.5*(self.a + self.b)

    def approximate_radius(self):
        center = self.approximate_center()
        return center.dist(self.a)

    def contains(self, p):
        # TODO numerically robust comparison
        center = self.approximate_center()
        radius = self.approximate_radius()
        return p.dist2(center) <= radius*radius

    def draw(self, **kwargs):
        center = self.approximate_center()
        radius = self.approximate_radius()
        draw_circle(center.x, center.y, radius, **kwargs)

def smallest_enclosing_circle_bruteforce(points):
    smallest_circle = None
    
    for i in range(len(points)):
        a = points[i]
        
        for j in range(i + 1, len(points)):
            b = points[j]
            
            for k in range(j + 1, len(points)):
                c = points[k]

                try:
                    circle = CircleFromThreePoints(a, b, c)
                except ValueError:
                    continue

                # circle must contain all points
                if all(circle.contains(p) for p in points):
                    # reads as: "if no smallest circle exists yet or
                    # the radius of the smallest circle so far is greater than the radius
                    # of the current circle"
                    if smallest_circle is None or smallest_circle.compare_radius(circle) > 0:
                        smallest_circle = circle

                # draw all possible circles
                #circle.draw(color='gray')
    
    for i in range(len(points)):
        a = points[i]
        
        for j in range(i + 1, len(points)):
            b = points[j]

            circle = CircleFromTwoPoints(a, b)

            if all(circle.contains(p) for p in points):
                # TODO numerically robust comparison between radii of CircleFromThree/TwoPoints
                if smallest_circle is None or smallest_circle.approximate_radius() > circle.approximate_radius():
                    smallest_circle = circle
    
    return smallest_circle

def random_point(r=100):
    x = random.randint(-r, r)
    y = random.randint(-r, r)

    return Point(x, y)

points = []
while len(points) < 10:
    p = random_point()

    if p not in points:
        points.append(p)

smallest_circle = smallest_enclosing_circle_bruteforce(points)

print("approximate radius:", smallest_circle.approximate_radius())

smallest_circle.draw(color='blue')

# draw points
x = [p.x for p in points]
y = [p.y for p in points]
plt.plot(x, y, 'ro', markeredgecolor='black')

# set bounds of plot
x0 = min(x)
y0 = min(y)
x1 = max(x)
y1 = max(y)
dx = x1 - x0
dy = y1 - y0
s = 0.5
x0 -= dx*s
x1 += dx*s
y0 -= dy*s
y1 += dy*s
plt.xlim([x0, x1])
plt.ylim([y0, y1])
plt.show()
