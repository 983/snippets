import numpy as np
import scipy.misc
import os

def flatten(list_of_lists):
    return [value for values in list_of_lists for value in values]

cube_x = np.float32([-1, +1, +1, -1, +1, -1, -1, +1])
cube_y = np.float32([-1, -1, -1, -1, +1, +1, +1, +1])
cube_z = np.float32([-1, -1, +1, +1, -1, -1, +1, +1])

cube_quad_indices = [
    [0, 1, 2, 3],
    [1, 4, 7, 2],
    [4, 5, 6, 7],
    [5, 0, 3, 6],
    [3, 2, 7, 6],
    [1, 0, 5, 4],
]

cube_tri_indices = flatten(
    [a, b, c, a, c, d]
    for a,b,c,d in cube_quad_indices)

cube_vertex_positions = np.float32(flatten(
    [[cube_x[i], cube_y[i], cube_z[i]]
     for i in cube_tri_indices]))

cube_texture_coordinates = np.float32([
    0, 0, 1, 0, 1, 1, # lower right triangle
    0, 0, 1, 1, 0, 1, # upper left triangle
]*6)

def make_projection(l, r, b, t, n, f):
    return np.float32([
        [2*n/(r - l), 0, (r + l)/(r - l), 0],
        [0, 2*n/(t - b), (t + b)/(t - b), 0],
        [0, 0, (f + n)/(n - f), 2*f*n/(n - f)],
        [0, 0, -1, 0],
    ])

def make_rotx(radians):
    c = np.cos(radians)
    s = np.sin(radians)
    
    A = np.eye(4, 4, dtype=np.float32)
    
    A[1, 1] = c
    A[1, 2] = -s
    A[2, 1] = s
    A[2, 2] = c
    
    return A

def make_roty(radians):
    c = np.cos(radians)
    s = np.sin(radians)
    
    A = np.eye(4, 4, dtype=np.float32)
    
    A[0, 0] = c
    A[0, 2] = -s
    A[2, 0] = s
    A[2, 2] = c
    
    return A

def make_rotz(radians):
    c = np.cos(radians)
    s = np.sin(radians)
    
    A = np.eye(4, 4, dtype=np.float32)
    
    A[0, 0] = c
    A[0, 1] = -s
    A[1, 0] = s
    A[1, 1] = c
    
    return A

def make_perspective(fovy, aspect, near, far):
    f = 1/np.tan(0.5*np.deg2rad(fovy))
    nf = 1/(near - far)

    A = np.zeros((4, 4), dtype=np.float32)
    
    A[0, 0] = f/aspect
    A[1, 1] = f
    A[2, 2] = (far + near)*nf
    A[2, 3] = -1
    A[3, 2] = 2*far*near*nf

    return A

def cross(u, v):
    return np.float32([
        u[1]*v[2] - u[2]*v[1],
        u[2]*v[0] - u[0]*v[2],
        u[0]*v[1] - u[1]*v[0],
    ])

def length(u):
    return np.sqrt(np.sum(u*u))

def normalize(u):
    assert(length(u) > 1e-10)
    return 1.0/length(u)*u

def dot(a, b):
    return np.sum(a*b)

def make_look_at(camera, target, up):
    up = normalize(up)

    z = normalize(camera - target)
    x = normalize(cross(up, z))
    y = normalize(cross(z, x))
    
    M = np.eye(4, 4, dtype=np.float32)
    
    M[0, 0:3] = x
    M[1, 0:3] = y
    M[2, 0:3] = z
    M[0, 3] = -dot(x, camera)
    M[1, 3] = -dot(y, camera)
    M[2, 3] = -dot(z, camera)

    return M

def load_image(path):
    image = scipy.misc.imread(path)
    image = np.flipud(image)
    return image

def load_images(directory, extensions=["jpg", "png", "bmp", "tga"]):
    images = []
    filenames = sorted(os.listdir(directory))
    for filename in filenames:
        filepath = os.path.join(directory, filename)
        
        ext = os.path.splitext(filename)[-1][1:]
        
        if ext not in extensions:
            print("Skipping file %s because extension %s not recognized as image"%(filepath, ext))
            continue
        
        image = load_image(filepath)
        
        images.append(image)
    
    return images

def split_n(values, n):
    result = []
    m = len(values)//n
    for i in range(n):
        result.append(values[i*m:(i+1)*m])
    return result
