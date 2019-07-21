from PIL import Image
import numpy as np
import random
import math
import time

def is_point_in_polygon(px, py, points):
    inside = False
    
    ax, ay = points[-1]
    for bx, by in points:
        
        # if points are on different sides
        if (ay < py) != (by < py):
            pax = px - ax
            pay = py - ay
            bax = bx - ax
            bay = by - ay
            
            if bay < 0:
                bax = -bax
                bay = -bay
            
            if pax * bay < pay * bax:
                inside = not inside
        
        ax = bx
        ay = by
    
    return inside

def rasterize_slow(image, polygon, color):
    dx = -0.5
    dy = -0.5
    
    polygon = [(x + dx, y + dy) for x, y in polygon]
    
    for y in range(image.shape[0]):
        for x in range(image.shape[1]):
            if is_point_in_polygon(x, y, polygon):
                image[y, x] = color

def draw_line(image, ax, ay, bx, by, color):
    dx = bx - ax
    dy = by - ay
    n = int(1 + 2*np.sqrt(dx*dx + dy*dy))
    
    for u in np.linspace(0, 1, n):
        x = ax + u*(bx - ax)
        y = ay + u*(by - ay)
        x = int(x)
        y = int(y)
        
        if 0 <= x < image.shape[1] and 0 <= y < image.shape[0]:
            image[y, x] = color

class Frac(object):
    def __init__(self, a, b):
        if b < 0:
            a = -a
            b = -b
        
        self.a = a
        self.b = b
    
    def __lt__(self, other):
        return self.a * other.b < other.a * self.b

    def to_int(self):
        return int(math.ceil(self.a / self.b))


def rasterize_fast(image, polygon, color):
    dx = -0.5
    dy = -0.5
    
    polygon = [(x + dx, y + dy) for x, y in polygon]
    
    ny, nx = image.shape[:2]
    
    intersections = [[] for _ in range(ny)]
    
    ax, ay = polygon[-1]
    for bx, by in polygon:
        
        y0 = int(math.floor(min(ay, by)))
        y1 = int(math.floor(max(ay, by)))
        y0 = max(0, min(ny - 1, y0))
        y1 = max(0, min(ny - 1, y1))
        
        for py in range(y0, y1 + 1):
            if (ay < py) != (by < py):
                pay = py - ay
                bax = bx - ax
                bay = by - ay
                
                x = Frac(pay*bax + ax*bay, bay)
                
                intersections[py].append(x)
        
        ax = bx
        ay = by
    
    for y, xs in enumerate(intersections):
        xs.sort()
        for i in range(0, len(xs), 2):
            x0 = xs[i + 0].to_int()
            x1 = xs[i + 1].to_int()
            image[y, x0:x1] = 1

def test_rasterizer(rasterize):
    nx = 16
    ny = 8
    
    polygons = [
        [(9.5, -1.5), (10.5, 0.5), (9.5, 0.5)],
        [(13.5, 0.5), (15.5, 0.5), (15.5, 2.5)],
        [(13.5, 0.5), (15.5, 2.5), (13.5, 2.5)],
        [(11.5, 1.5), (12.5, 2.5), (11.5, 3.5)],
        [(8, 1), (7, 4), (5, 2)],
        [(8, 1), (9.5, 2.5), (7, 4)],
        [(1, 2), (5, 2), (7, 4)],
        [(9.5, 2.75), (11.75, 5.5), (7.75, 5.5)],
        [(11.75, 5.5), (9.75, 7.75), (7.75, 5.5)],
        [(14.5, 3.5), (14.5, 5.5), (13.5, 6.5)],
        [(14.5, 5.5), (15, 8), (13.5, 6.5)],
        [(2, 4), (6, 6), (1, 7)],
        [(6.5, 6.5), (7.5, 6.5), (7.5, 7.5)],
        [(5.25, 6.75), (6.25, 6.75), (6.25, 7.75)],
        [(4.5, 7.5)]*3,
    ]

    image = np.zeros((ny, nx))
    for polygon in polygons:
        rasterize(image, polygon, 1.0)
    
    expected = np.array(Image.open("expected.png"))[::-1, :, 0]/255.0
    
    difference = np.max(image - expected)
    assert(difference == 0.0)
    
    if 0:
        scale = 100
        
        image = np.repeat(image, scale, axis=0)
        image = np.repeat(image, scale, axis=1)
        image = np.stack([image]*3, axis=2)
        
        for polygon in polygons:
            ax, ay = polygon[-1]
            for bx, by in polygon:
                draw_line(image, scale*ax, scale*ay, scale*bx, scale*by, (0, 1, 0))
                
                ax = bx
                ay = by
        
        image = Image.fromarray(np.clip(image[::-1]*255, 0, 255).astype(np.uint8))
        image.show()

def compare_rasterizers():

    nx = 640
    ny = 480
    
    polygon = []
    
    random.seed(123456789)
    
    for _ in range(30):
        grid = 10
        x = int(random.random() * grid) * nx // (grid - 1)
        y = int(random.random() * grid) * ny // (grid - 1)
        
        polygon.append((x, y))
    
    image1 = np.zeros((ny, nx))
    image2 = np.zeros((ny, nx))
    
    t0 = time.perf_counter()
    
    rasterize_fast(image1, polygon, 1)
    
    t1 = time.perf_counter()
    
    rasterize_slow(image2, polygon, 1)

    t2 = time.perf_counter()
    
    image3 = np.abs(image1 - image2)
    
    print(t1 - t0, "seconds")
    print(t2 - t1, "seconds")
    
    difference = np.sum(image3)
    
    print("difference:", difference)
    
    assert(difference == 0.0)
    
    image = np.concatenate([
        image1,
        image2,
        image3,
    ], axis=1)
    
    image = np.stack([image]*3, axis=2)
    
    ax, ay = polygon[-1]
    for bx, by in polygon:
        draw_line(image[:, 0*nx:1*nx, :], ax, ay, bx, by, (0, 1, 0))
        draw_line(image[:, 1*nx:2*nx, :], ax, ay, bx, by, (0, 1, 0))
        draw_line(image[:, 2*nx:3*nx, :], ax, ay, bx, by, (0, 1, 0))
        
        ax = bx
        ay = by
    
    image = Image.fromarray(np.clip(image[::-1]*255, 0, 255).astype(np.uint8))
    image.show()
    
def test():
    test_rasterizer(rasterize_slow)
    test_rasterizer(rasterize_fast)
    compare_rasterizers()

test()
