import numpy as np
from numba import njit
import matplotlib.pyplot as plt
import time, sys
import itertools
from scipy.ndimage.morphology import distance_transform_edt as edt

@njit
def calculate_df_sweep(mask, neighbors):
    height, width = mask.shape

    df = np.zeros((height, width))

    for y in range(height):
        for x in range(width):
            if mask[y, x]: continue

            df[y, x] = np.inf
            for dx in range(-1, 2):
                for dy in range(-1, 2):
                    x2 = x + dx
                    y2 = y + dy
                    if x2 >= 0 and x2 < width and y2 >= 0 and y2 < height:
                        if mask[y, x] != mask[y2, x2]:
                            df[y, x] = min(df[y, x], np.sqrt(dx * dx + dy * dy))

    distances = np.sqrt(neighbors[:, 0]**2 + neighbors[:, 1]**2)

    for y in range(height):
        for x in range(width):
            for i in range(len(neighbors)):
                x2 = x + neighbors[i, 0]
                y2 = y + neighbors[i, 1]
                if x2 >= 0 and x2 < width and y2 >= 0 and y2 < height:
                    df[y, x] = min(df[y, x], df[y2, x2] + distances[i])

    for y in range(height - 1, -1, -1):
        for x in range(width - 1, -1, -1):
            for i in range(len(neighbors)):
                x2 = x - neighbors[i, 0]
                y2 = y - neighbors[i, 1]
                if x2 >= 0 and x2 < width and y2 >= 0 and y2 < height:
                    df[y, x] = min(df[y, x], df[y2, x2] + distances[i])

    return df
"""
# BUG: inaccurate
def edt1d(grid, offset, stride, length, f, v, z):
    v[0] = 0
    z[0] = -np.inf
    z[1] = np.inf

    for q in range(length):
        f[q] = grid[offset + q * stride];

    k = 0
    s = 0
    for q in range(1, length):
        while True:
            r = v[k];
            s = (f[q] - f[r] + q * q - r * r) / (q - r) / 2;

            if s >= z[k]: break
            k -= 1

            if k < 0: break

        k += 1
        v[k] = q
        z[k] = s
        z[k + 1] = np.inf

    k = 0
    for q in range(length):
        while z[k + 1] < q:
            k += 1

        r = v[k]

        grid[offset + q * stride] = f[r] + (q - r) * (q - r);

def calculate_df_meijster_old(mask, neighbors):
    height, width = mask.shape
    df = np.full(mask.shape, fill_value=np.inf)

    for y in range(height):
        for x in range(width):
            if mask[y, x]: continue

            df[y, x] = np.inf
            for dx in range(-1, 2):
                for dy in range(-1, 2):
                    x2 = x + dx
                    y2 = y + dy
                    if 0 <= x2 < width and 0 <= y2 < height:
                        if mask[y, x] != mask[y2, x2]:
                            df[y, x] = min(df[y, x], np.sqrt(dx * dx + dy * dy))

    data = df.ravel()

    n = max(height, width)

    f = np.zeros(n)
    z = np.zeros(n + 1)
    v = np.zeros(n, dtype=np.int32)

    for x in range(width): edt1d(data, x, width, height, f, v, z);
    for y in range(height): edt1d(data, y * width, 1, width, f, v, z);

    df[mask] = 0

    return np.sqrt(df)
"""

@njit
def calculate_df_naive(mask):
    h, w = mask.shape

    df = np.full(mask.shape, np.inf)

    for y in range(h):
        #print(y + 1, "of", h)
        for x in range(w):

            if mask[y, x]:
                df[y, x] = 0
                continue

            for y2 in range(h):
                for x2 in range(w):
                    if mask[y, x] != mask[y2, x2]:
                        dx = x - x2
                        dy = y - y2
                        d = np.sqrt(dx * dx + dy * dy)
                        df[y, x] = min(df[y, x], d)

    return df

@njit("(f8, f8, f8)", cache=True)
def df_parabola(x0, dy0, x):
    dx0 = x - x0
    return dx0*dx0 + dy0*dy0

@njit("(f8[:],)", cache=True)
def propagate_1d_first_pass(d):
    n = len(d)
    for i1 in range(1, n):
        i2 = i1 - 1
        d[i1] = min(d[i1], d[i2] + 1)

    for i1 in range(n - 2, -1, -1):
        i2 = i1 + 1
        d[i1] = min(d[i1], d[i2] + 1)

@njit("(f8[:], i4[:], f8[:], f8[:])", cache=True)
def propagate_1d(d, v, z, f):
    nx = len(d)
    k = -1
    s = -np.inf
    for x in range(nx):
        d_yx = d[x]

        if d_yx == np.inf: continue

        fx = d_yx * d_yx

        f[x] = fx

        while k >= 0:
            vk = v[k]

            s = 0.5 * (fx + x*x - f[vk] - vk * vk) / (x - vk)

            if s > z[k]: break

            k -= 1

        k += 1
        v[k] = x
        z[k] = s
        z[k + 1] = np.inf

    if k < 0: return

    k = 0
    for x in range(nx):
        while z[k + 1] < x:
            k += 1

        vk = v[k]
        dx = x - vk

        d[x] = np.sqrt(dx * dx + f[vk])

@njit("(f8[:, :],)", cache=True)
def propagate_distances_felz(d):
    ny, nx = d.shape

    for x in range(nx):
        propagate_1d_first_pass(d[:, x])

    v = np.zeros(nx, dtype=np.int32)
    z = np.zeros(nx + 1)
    f = np.zeros(nx)

    for y in range(ny):
        propagate_1d(d[y], v, z, f)

def calculate_df_felz(mask):
    distances = np.full(mask.shape, fill_value=np.inf)
    distances[mask] = 0

    propagate_distances_felz(distances)

    return distances

@njit("(f8[:, :],)", cache=True)
def propagate_squared_distances_meijster(squared_distances):
    d = squared_distances
    ny, nx = d.shape

    parabola_x = np.zeros(nx + 1, dtype=np.int32)
    x_intersections = np.zeros(nx)
    temp_row_distances = np.zeros(nx)

    for x in range(nx):
        for y1 in range(1, ny):
            y2 = y1 - 1

            d[y1, x] = min(d[y1, x], d[y2, x] + 1)

        for y1 in range(ny - 2, -1, -1):
            y2 = y1 + 1
            d[y1, x] = min(d[y1, x], d[y2, x] + 1)

    for y in range(ny):
        # find first parabola
        n = -1
        for x1 in range(nx):
            if d[y, x1] < np.inf:
                n += 1
                parabola_x[n] = x1
                break

        # do nothing if entire row has infinite distance
        if n == -1: continue

        for x1 in range(x1 + 1, nx):
            dy1 = d[y, x1]

            if dy1 == np.inf: continue

            while n > 0:
                x0 = parabola_x[n]
                dy0 = d[y, x0]

                dfp0 = df_parabola(x0, dy0, x_intersections[n - 1])
                dfp1 = df_parabola(x1, dy1, x_intersections[n - 1])

                if dfp0 < dfp1: break

                n -= 1

            x0 = parabola_x[n]
            dy0 = d[y, x0]
            x_intersections[n] = 0.5 / (x1 - x0) * (x1 * x1 + dy1 * dy1 - x0 * x0 - dy0 * dy0)
            n += 1
            parabola_x[n] = x1

        for x1 in range(nx - 1, -1, -1):
            while n > 0 and x1 < x_intersections[n - 1]:
                n -= 1

            x0 = parabola_x[n]
            dy0 = d[y, x0]

            temp_row_distances[x1] = df_parabola(x0, dy0, x1)

        for x1 in range(nx):
            d[y, x1] = temp_row_distances[x1]

def calculate_df_meijster(mask):
    squared_distances = np.full(mask.shape, fill_value=np.inf)
    squared_distances[mask] = 0

    propagate_squared_distances_meijster(squared_distances)

    distances = np.sqrt(squared_distances)

    return distances

class Timer:
    def __init__(self):
        self.t = time.perf_counter()

    def stop(self, message=None):
        dt = time.perf_counter() - self.t

        if message:
            print(f"{message} - {dt * 1000:9.3f} milliseconds")

        self.t = time.perf_counter()

        return dt

def test():
    for i_test in range(1000):
        h = np.random.randint(1, 100)
        w = np.random.randint(1, 100)
        mask = np.random.rand(h, w) < 0.02

        if np.sum(mask) == 0:
            continue

        t = Timer()

        df_scipy = edt(np.logical_not(mask))

        t.stop("scipy   ")

        df_meijster = calculate_df_meijster(mask)

        t.stop("meijster")

        df_felz = calculate_df_felz(mask)

        t.stop("felz    ")
        print("")

        #df_true = calculate_df_naive(mask)

        assert(np.max(np.abs(df_scipy - df_meijster)) < 1e-10)
        assert(np.max(np.abs(df_scipy - df_felz)) < 1e-10)

        if i_test % 100 == 0:
            print(i_test)

    print("tests passed")

def main():
    np.random.seed(0)

    test()
    exit(0)

    n = 32

    mask = np.random.rand(n, n) < 0.02

    df_true = calculate_df_naive(mask)

    neighbors = np.array([[-1, -1], [0, -1], [1,  -1], [-1, 0]])

    df_sweep = calculate_df_sweep(mask, neighbors)

    df_scipy = edt(np.logical_not(mask))

    df_meijster = calculate_df_meijster(mask)

    df_felz = calculate_df_felz(mask)

    dfs = [
        (df_sweep, "df_sweep"),
        (df_scipy, "df_scipy"),
        (df_meijster, "df_meijster"),
        (df_felz, "df_felz"),
    ]

    for df, name in dfs:
        print(name, "mean error: %e" % np.mean(np.abs(df - df_true)))

    images = [
        (mask, "mask"),
        (df_true, "df_true"),
        (df_meijster, "df_meijster"),
        (df_felz, "df_felz"),
        (np.abs(df_sweep - df_true), "|sweep - true|"),
        (np.abs(df_scipy - df_true), "|scipy - true|"),
        (np.abs(df_meijster - df_true), "|meijster - true|"),
        (np.abs(df_felz - df_true), "|felz - true|"),
    ]

    for i, (image, name) in enumerate(images):
        plt.subplot(2, 4, 1 + i)
        plt.title(name)
        plt.imshow(image, cmap='gray', interpolation='nearest')
        plt.axis('off')

    plt.show()

main()
