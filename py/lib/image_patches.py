import threading
from pathlib import Path
from PIL import Image
import cv2
import random
import time
import os
import numpy as np
from util import random_subimage

directory = r"C:\Users\owl\Desktop\projects\datasets\pixabay downloader"

with open(os.path.join(directory, "paths.txt"), "r") as f:
    paths = f.read().split("\n")

weights = [np.load(r"C:\lib\W_%d.npy"%i) for i in range(3)]

def load_random_image():
    while True:
        path = random.choice(paths)
        parts = Path(path).parts
        path = os.path.join(directory, *parts)
        
        try:
            image = Image.open(path)
        except Exception as e:
            print(e, path)
            continue
        
        image = np.array(image)

        if len(image.shape) == 3 and image.shape[2] == 3:
            return image

def is_interesting(image):
    image = cv2.resize(image, (8, 8))
    X = image.reshape(1, 8*8*3)
    X = X@weights[0]
    X = np.maximum(0, X)
    X = X@weights[1]
    X = np.maximum(0, X)
    X = X@weights[2]
    return np.round(X)

def update_cache_thread():
    while True:
        # add random image
        image = load_random_image()
        cached_images.append(image)

        # remove random image
        i = random.randrange(len(cached_images))
        cached_images[i] = cached_images[-1]
        cached_images.pop()

        time.sleep(0.1)

cached_images = [load_random_image() for _ in range(100)]

thread = threading.Thread(target=update_cache_thread)
thread.start()

def make_patches(batch_size, width, height):
    depth = 3
    inputs = np.zeros((batch_size, height, width, depth), dtype=np.float32)

    for i in range(batch_size):
        while True:
            try:
                image = random.choice(cached_images)
            
                patch = random_subimage(image, width, height)[0]

                patch = patch.astype(np.float32)/255.0

                if is_interesting(patch):
            
                    inputs[i] = patch
                
                    break
            except Exception as e:
                print(e)

    return inputs

if __name__ == '__main__':
    from util import show
    
    patches = make_patches(3*3, 64, 64)

    show(patches)
