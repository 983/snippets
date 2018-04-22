import numpy as np
import random, time, sys, os
import matplotlib.pyplot as plt
from numpy.lib.stride_tricks import as_strided
from PIL import Image
import sounddevice as sd

def make_grid(images, ny, nx):
    if len(images[0].shape) == 2:
        height, width = images[0].shape
        result = np.zeros((height*ny, width*nx))
    else:
        height, width, depth = images[0].shape
        result = np.zeros((height*ny, width*nx, depth))
    
    k = 0
    for y in range(0, result.shape[0], height):
        for x in range(0, result.shape[1], width):
            result[y:y+height, x:x+width] = images[k]
            k += 1
    return result

def down_nearest(larger):
    height, width = larger.shape[:2]
    
    if len(larger.shape) == 2:
        smaller = np.zeros((height//2, width//2))
    else:
        smaller = np.zeros((height//2, width//2, larger.shape[2]))
        
    smaller[:, :] = larger[::2, ::2]
    
    return smaller

def up_nearest(smaller):
    height, width = smaller.shape[:2]
    
    if len(smaller.shape) == 2:
        larger = np.zeros((height*2, width*2))
    else:
        larger = np.zeros((height*2, width*2, smaller.shape[2]))
    
    for y in range(2):
        for x in range(2):
            larger[y:y+height*2:2, x:x+width*2:2] = smaller
            
    return larger

def is_rgb(image):
    return len(image.shape) == 3 and image.shape[2] == 3

def random_subimage(image, width, height):
    x = random.randrange(image.shape[1] - width)
    y = random.randrange(image.shape[0] - height)
    subimage = image[y:y+height, x:x+width]
    return subimage, x, y

def notify(n_sec=None):
    samplerate = 48*1000
    t = np.linspace(0, 2*np.pi, samplerate, endpoint=False)

    y = np.concatenate([
        np.cos(440*t),
        np.cos(880*t),
    ]).astype(np.float32)

    def callback(outdata, frames, time, status):
        outdata[:] = y

    stream = sd.RawOutputStream(
        samplerate=samplerate,
        blocksize=len(y),
        channels=1,
        dtype='float32',
        callback=callback)

    with stream:
        if n_sec is None:
            while True:
                time.sleep(1)
        else:
            time.sleep(n_sec)

def pad(image, padding):
    if isinstance(padding, int):
        pad_x = padding
        pad_y = padding
    else:
        pad_y, pad_x = padding
    
    height, width, depth = image.shape
    padded_image = np.zeros((height + 2*pad_y, width + 2*pad_x, depth))
    padded_image[pad_y:pad_y+height, pad_x:pad_x+width] = image
    return padded_image

def pos_to_diag(i, j):
    # Given a position (i, j) in a matrix,
    # what is the diagonal and the offset in the diagonal?
    
    k = j - i
    offset = max(i, j) - abs(k)
    return (k, offset)

def diag_to_pos(k, offset):
    # Given a diagonal and an offset in that diagonal,
    # what is the position in the matrix?
    i = offset - min(0, k)
    j = offset + max(0, k)
    return (i, j)

def flatten_once(values):
    result = []
    for value in values:
        result.extend(value)
    return result

def interleave(*arrays):
    return flatten_once(zip(*arrays))

def list_dir(directory):
    for path in os.listdir(directory):
        path = os.path.join(directory, path)
        
        if os.path.isfile(path):
            yield path

        if os.path.isdir(path):
            yield from list_dir(path)

def load_image_u8(path, flatten=False):
    image = Image.open(path)
    image = np.array(image, dtype=np.uint8)
    
    if flatten and len(image.shape) == 3:
        image = image[:, :, :3]
        image = np.mean(image, axis=2)
    
    return image

def load_image(path, flatten=False):
    image = Image.open(path)
    image = np.array(image, dtype=np.float32)
    image = image/255.0
    if flatten and len(image.shape) == 3:
        image = image[:, :, :3]
        image = np.mean(image, axis=2)
    return image

def save_image(path, image):
    if image.dtype in [np.float32, np.float64]:
        image = np.clip(image*255.0, 0, 255).astype(np.uint8)

    if image.dtype != np.uint8:
        raise ValueError("Invalid image dtype:", image.dtype)

    image = Image.fromarray(image)

    image.save(path)

def make_clamped_windows(image, window_size=(3,3)):
    wy, wx = window_size
    height, width = image.shape
    rx = wx//2
    ry = wy//2

    x = np.arange(width)
    y = np.arange(height)
    
    x, y = np.meshgrid(x, y)
    
    windows = np.array([
        image[
            np.clip(y + dy, 0, height - 1),
            np.clip(x + dx, 0, width - 1)]
        for dy in range(-ry, ry + 1)
        for dx in range(-rx, rx + 1)
    ]).reshape([wx*wy, width*height]).T
    
    return windows

def rolling_block(A, block=(3, 3)):
    shape = (A.shape[0] - block[0] + 1, A.shape[1] - block[1] + 1) + block
    strides = (A.strides[0], A.strides[1]) + A.strides
    return as_strided(A, shape=shape, strides=strides)

def make_windows(image, window_size=(3, 3)):
    height, width = image.shape
    wy, wx = window_size
    
    windows = np.array([
        image[y:y+height-wy+1, x:x+width-wx+1].flatten()
        for y in range(wy)
        for x in range(wx)
    ]).T

    return windows

def lerp(a, b, u):
    return a*(1 - u) + u*b

def smoothstep(a, b, x):
    t = np.clip((x - a)/(b - a), 0.0, 1.0)
    return t*t*(3.0 - 2.0*t)

def blend(src, dst, alpha):
    return src*alpha + (1 - alpha)*dst

def flatten(tensors):
    return np.concatenate([tensor.flatten() for tensor in tensors])

def unflatten(flattened, shapes):
    i = 0
    tensors = []
    for shape in shapes:
        n = np.product(shape)
        tensor = flattened[i:i+n].reshape(shape)
        i += n
        tensors.append(tensor)
    assert(i == len(flattened))
    return tensors

def normalize(values):
    a = np.min(values)
    b = np.max(values)
    if b - a == 0:
        print("Note: constant color image")
        return values*0
    return (values - a)/(b - a)

def show(images, titles=None, **kwargs):
    try:
        if len(images.shape) == 2 or (len(images.shape) == 3 and images.shape[2] in [1, 3, 4]):
            show([images])
            return
    except Exception:
        pass
    
    n = len(images)

    if "nx" in kwargs and "ny" in kwargs:
        nx = kwargs["nx"]
        ny = kwargs["ny"]
        del kwargs["nx"]
        del kwargs["ny"]
    else:
        nx = 1
        ny = 1

        while nx*ny < n:
            if nx <= ny:
                nx += 1
            else:
                ny += 1

    for i, image in enumerate(images):
        if image.dtype == np.uint8:
            image = image.astype(np.float64)/256.0
        
        a = np.min(image)
        b = np.max(image)

        if 0 <= a <= b <= 1:
            pass
        else:
            image = image.astype(np.float64)
            image = (image - a)/(b - a)

        plt.subplot(ny, nx, 1 + i)

        if titles is not None:
            plt.title(titles[i])
        
        plt.imshow(image, cmap='gray', vmin=0, vmax=1, **kwargs)
        plt.axis('off')

    plt.show()
