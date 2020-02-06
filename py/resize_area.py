import cv2
import numpy as np

np.random.seed(0)

def make_pairs(n_old, n_new):
    pairs = [[] for _ in range(n_new)]
    
    left = 0.0
    for i_dst in range(n_new):
        right = (i_dst + 1)*n_old/n_new
        
        i0 = int(np.floor(left))
        i1 = int(np.ceil(right))
        
        for i in range(i0, min(i1, n_old)):
            # intersect dst range [left, right] with pixel range [i, i + 1]
            weight = min(right, i + 1) - max(left, i)
            
            pairs[i_dst].append((i, weight))
        
        left = right
    
    return pairs

def resize_area(image, new_width, new_height):
    old_height, old_width = image.shape[:2]
    
    result = np.zeros((new_height, new_width))
    
    x_pairs = make_pairs(old_width, new_width)
    y_pairs = make_pairs(old_height, new_height)
    
    normalization_factor = float(new_width * new_height) / float(old_width * old_height)
    
    for y in range(new_height):
        for x in range(new_width):
            s = 0.0
            
            for iy,wy in y_pairs[y]:
                for ix,wx in x_pairs[x]:
                    s += image[iy, ix] * wx * wy
            
            result[y, x] = normalization_factor * s
    
    return result
    
total = 0.0

def test(w0, h0, w1, h1):
    a = np.random.rand(w0, h0)

    b = resize_area(a, w1, h1)
    c = cv2.resize(a, (w1, h1), interpolation=cv2.INTER_AREA)
    
    difference = np.abs(b - c)
    
    global total
    total += difference.mean()
    
    print("max: %f mean: %f shape:"%(difference.max(), difference.mean()), w0, h0, w1, h1)

for _ in range(100):
    while True:
        w0,h0,w1,h1 = np.random.randint(10, 30, size=4)
        if w0 >= w1 and h0 >= h1: break
    
    test(w0, h0, w1, h1)

print("total: %f"%total)
