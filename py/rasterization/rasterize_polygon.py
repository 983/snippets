import numpy as np
from PIL import Image

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

def rasterize(image, polygon, color):
    for y in range(image.shape[0]):
        for x in range(image.shape[1]):
            dx = 0.5
            dy = 0.5
            if is_point_in_polygon(x + dx, y + dy, polygon):
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

def test():
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

test()
