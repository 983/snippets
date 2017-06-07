import numpy as np
import matplotlib.pyplot as plt
from skimage.io import imread
from skimage.transform import iradon_sart
from numba import jit
import time

@jit(nopython=True, nogil=True)
def lerp(a, b, u):
    return a + u*(b - a)

@jit(nopython=True, nogil=True)
def i2i(x, a, b, c, d):
    u = (x - a)/(b - a)
    return c + u*(d - c)

@jit(nopython=True, nogil=True)
def get_image_at(image, ix, iy):
    height, width = image.shape
    if ix >= 0 and ix < width and iy >= 0 and iy < height:
        return image[iy, ix]
    return 0.0

@jit(nopython=True, nogil=True)
def add_image_at(image, ix, iy, value):
    height, width = image.shape
    if ix >= 0 and ix < width and iy >= 0 and iy < height:
        image[iy, ix] += value

@jit(nopython=True, nogil=True)
def get_image_at_nearest(image, x, y):
    return get_image_at(image, int(x), int(y))

@jit(nopython=True, nogil=True)
def get_image_at_smooth(image, x, y):
    ix = int(x)
    iy = int(y)

    ux = x - ix
    uy = y - iy

    value00 = get_image_at(image, ix + 0, iy + 0)*(1 - ux)*(1 - uy)
    value01 = get_image_at(image, ix + 0, iy + 1)*(1 - ux)*uy
    value10 = get_image_at(image, ix + 1, iy + 0)*ux*(1 - uy)
    value11 = get_image_at(image, ix + 1, iy + 1)*ux*uy

    return value00 + value01 + value10 + value11

@jit(nopython=True, nogil=True)
def add_image_at_smooth(image, x, y, value):
    ix = int(x)
    iy = int(y)

    ux = x - ix
    uy = y - iy

    add_image_at(image, ix + 0, iy + 0, value*(1 - ux)*(1 - uy))
    add_image_at(image, ix + 0, iy + 1, value*(1 - ux)*     uy )
    add_image_at(image, ix + 1, iy + 0, value*     ux *(1 - uy))
    add_image_at(image, ix + 1, iy + 1, value*     ux *     uy )

@jit(nopython=True, nogil=True)
def sum_line(image, start_x, start_y, end_x, end_y, n):
    line_integral = 0.0
    
    for u in np.linspace(0, 1, n):
        x = lerp(start_x, end_x, u)
        y = lerp(start_y, end_y, u)

        value = get_image_at_smooth(image, x, y)
        
        line_integral += value

    return line_integral

@jit(nopython=True, nogil=True)
def draw_line(image, start_x, start_y, end_x, end_y, n, value):
    for u in np.linspace(0, 1, n):
        x = lerp(start_x, end_x, u)
        y = lerp(start_y, end_y, u)

        hamming_beta = 0.46
        hamming_window = ((1 - hamming_beta) - hamming_beta * np.cos(2*np.pi*u))

        add_image_at_smooth(image, x, y, value/n)#*hamming_window)

@jit(nopython=True, nogil=True)
def radon(image, more_rays_and_angles=1):
    # we make n_angles many steps around a circle
    n_angles = max(image.shape)*more_rays_and_angles
    # and at each point on the circle, we fire n_rays
    n_rays = max(image.shape)*more_rays_and_angles

    result = np.zeros((n_rays, n_angles))

    # center of circle
    center_x = image.shape[1]//2
    center_y = image.shape[0]//2

    # radius of circle
    r = min(image.shape)*0.5

    for j in range(n_angles):
        angle = i2i(j, 0, n_angles-1, 0.0, np.pi)

        c = np.cos(angle)
        s = np.sin(angle)

        # we fire a number of rays with origin on the same line segment
        # the center of that line segment is here
        line_center_x = center_x + c*r
        line_center_y = center_y + s*r

        t = np.linspace(-1, 1, n_rays)

        for i in range(n_rays):
            ti = i2i(i, 0, n_rays-1, -1, 1)

            # ray start position            
            start_x = line_center_x - s*r*ti
            start_y = line_center_y + c*r*ti

            # ray end position
            end_x = start_x - c*r*2
            end_y = start_y - s*r*2

            result[i, j] = sum_line(image, start_x, start_y, end_x, end_y, max(image.shape))

    return result

@jit(nopython=True, nogil=True)
def iradon(radon_transformed, image):
    n_rays   = radon_transformed.shape[0]
    n_angles = radon_transformed.shape[1]

    # center of circle
    center_x = image.shape[1]//2
    center_y = image.shape[0]//2

    # radius of circle
    r = min(image.shape)*0.5
    for j in range(n_angles):
        angle = i2i(j, 0, n_angles-1, 0.0, np.pi)

        c = np.cos(angle)
        s = np.sin(angle)

        # we fire a number of rays with origin on the same line segment
        # the center of that line segment is here
        line_center_x = center_x + c*r
        line_center_y = center_y + s*r

        t = np.linspace(-1, 1, n_rays)

        for i in range(n_rays):
            ti = i2i(i, 0, n_rays-1, -1, 1)

            # ray start position            
            start_x = line_center_x - s*r*ti
            start_y = line_center_y + c*r*ti

            # ray end position
            end_x = start_x - c*r*2
            end_y = start_y - s*r*2

            before = sum_line(image, start_x, start_y, end_x, end_y, max(image.shape))

            value = transformed[i, j] - before

            draw_line(image, start_x, start_y, end_x, end_y, max(image.shape), value)
    
    return image

@jit(nopython=True, nogil=True)
def sign(x):
    if x < 0: return -1
    if x > 0: return +1
    return 0

if 1:
    x0 = np.pi
    y0 = np.e

    x1 = 10.5
    y1 = 7.3

    dx = x1 - x0
    dy = y1 - y0

    distance = np.sqrt(dx*dx + dy*dy)

    vx = dx / distance
    vy = dy / distance

    ix = np.floor(x0)
    iy = np.floor(y0)

    sx = sign(dx)
    sy = sign(dy)

    tx = (ix - x0 + float(dx >= 0))/vx
    ty = (iy - y0 + float(dy >= 0))/vy
    
    stx = sx/vx
    sty = sy/vy

    for x in range(10):
        plt.plot([x, x], [0, 10])
    for y in range(10):
        plt.plot([0, 10], [y, y])

    while True:
        t = min(tx, ty)

        if tx >= distance:
            break

        if tx < ty:
            tx += stx
            ix += sx
        else:
            ty += sty
            iy += sy

        x = x0 + vx*t
        y = y0 + vy*t

        print(x, y)
        plt.plot([x], [y], 'ro')
        plt.plot([ix], [iy], 'bx')

    plt.show()
"""

image = imread("test.png", as_grey=True)

transformed = radon(image)
transformed += np.random.randn(*transformed.shape)

reconstructed = np.zeros(image.shape)
for _ in range(10):
    start_time = time.clock()
    reconstructed = iradon(transformed, reconstructed)
    delta_time = time.clock() - start_time
    print("%f seconds"%delta_time)
    reconstructed = np.clip(reconstructed, 0, 1)

plt.subplot(1, 3, 1)
plt.imshow(image, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.subplot(1, 3, 2)
plt.imshow(transformed, cmap='gray')
plt.axis('off')

plt.subplot(1, 3, 3)
plt.imshow(reconstructed, cmap='gray', interpolation='nearest')
plt.axis('off')

plt.show()


    
"""
