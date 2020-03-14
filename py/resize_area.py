import cv2
import numpy as np
import scipy.sparse

np.random.seed(0)

def make_matrix(n, m):
    left = 0

    i_inds = []
    j_inds = []
    weights = []
    
    for i in range(n):
        right = (i + 1) * m / n
        
        j0 = int(np.floor(left))
        j1 = int(np.ceil(right))
        
        for j in range(j0, j1):
            # intersect dst range [left, right] with pixel range [i, i + 1]
            weight = min(right, j + 1) - max(left, j)

            i_inds.append(i)
            j_inds.append(j)
            weights.append(weight)

        left = right

    A = scipy.sparse.csr_matrix((weights, (i_inds, j_inds)), shape=(n, m))

    return A

def resize_area(image, new_width, new_height):
    scale = (new_width * new_height) / np.product(image.shape)
    
    A0 = make_matrix(new_height, image.shape[0])
    A1 = make_matrix(new_width, image.shape[1])

    image = A0 @ image
    image = (A1 @ image.T).T

    return scale * image
    
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
