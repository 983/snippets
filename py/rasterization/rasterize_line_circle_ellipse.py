import numpy as np
from PIL import Image

#bresenham line, circle and ellipse rasterization

image = np.zeros((512, 512), dtype=np.uint8)

def set_pixel(x, y):
    image[y, x] = 255

def line(ax, ay, bx, by, set_pixel):
    dx = +abs(bx - ax)
    dy = -abs(by - ay)
    sx = 1 if ax < bx else -1
    sy = 1 if ay < by else -1
    
    err = dx + dy
    
    while True:
        set_pixel(ax, ay)
        
        if ax == bx and ay == by: return
        
        e2 = 2 * err
        
        if e2 > dy:
            err += dy
            ax += sx
        
        if e2 < dx:
            err += dx
            ay += sy

def ellipse(xm, ym, a, b, set_pixel):
    dx = 0
    dy = b
    a2 = a * a
    b2 = b * b
    err = b2 - (2 * b - 1) * a2
    
    while True:
        set_pixel(xm + dx, ym + dy)
        set_pixel(xm - dx, ym + dy)
        set_pixel(xm - dx, ym - dy)
        set_pixel(xm + dx, ym - dy)
        
        e2 = 2 * err
        
        if e2 < (2*dx + 1)*b2:
            dx += 1
            err += (2*dx + 1)*b2
        
        if e2 > -(2*dy - 1)*a2:
            dy -= 1
            err -= (2*dy - 1)*a2
        
        if dy < 0: break

    while dx < a:
        set_pixel(xm + dx, ym)
        set_pixel(xm - dx, ym)
        
        dx += 1

def circle(x0, y0, radius, set_pixel):
    f = 1 - radius
    dx = 0
    dy = -2 * radius
    x = 0
    y = radius
    
    set_pixel(x0, y0 + radius)
    set_pixel(x0, y0 - radius)
    set_pixel(x0 + radius, y0)
    set_pixel(x0 - radius, y0)
    
    while x < y:
        if f >= 0:
            y -= 1
            dy += 2
            f += dy
        
        x += 1
        dx += 2
        f += dx + 1
        
        set_pixel(x0 + x, y0 + y)
        set_pixel(x0 - x, y0 + y)
        set_pixel(x0 + x, y0 - y)
        set_pixel(x0 - x, y0 - y)
        set_pixel(x0 + y, y0 + x)
        set_pixel(x0 - y, y0 + x)
        set_pixel(x0 + y, y0 - x)
        set_pixel(x0 - y, y0 - x)

line(100, 200, 300, 400, set_pixel)

circle(100, 100, 50, set_pixel)

ellipse(250, 300, 100, 50, set_pixel)

Image.fromarray(image).show()
