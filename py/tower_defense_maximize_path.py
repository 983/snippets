import numpy as np
import matplotlib.pyplot as plt

def bfs(mask, start, stop):
    
    eight_neighborhood = [
        (-1, -1), (0, -1), (1, -1),
        (-1,  0),          (1,  0),
        (-1,  1), (0,  1), (1,  1),
    ]
    
    h, w = mask.shape
    prev = np.zeros((h, w, 2), dtype=np.int32)
    distances = np.full((h, w), np.inf)
    distances[0, 0] = 0
    
    points = [start]

    while len(points) > 0:
        #x, y = np.array(points).T
        #plt.scatter(x, y)
        
        neighbors = []
        
        for x, y in points:
            for dx, dy in eight_neighborhood:
                x2 = x + dx
                y2 = y + dy
                
                if 0 <= y2 < h and 0 <= x2 < w and not mask[y2, x2]:
                    
                    if abs(dx) + abs(dy) == 2 and (mask[y, x + dx] or mask[y + dy, x]):
                        continue
                    
                    dist = distances[y, x] + 1
                    
                    if distances[y2, x2] == np.inf:
                        distances[y2, x2] = dist
                        prev[y2, x2] = (x, y)
                        
                        neighbors.append((x2, y2))
        
        points = neighbors

    x, y = stop
    
    dist = distances[y, x]

    if dist == np.inf:
        return None, dist

    points = [stop]

    while x != start[0] or y != start[1]:
        x, y = prev[y, x]
        
        points.append((x, y))

    return points, dist

def main():
    w = 20
    h = 10
    
    start = (0, 0)
    stop = (w - 1, h - 1)
    
    while True:
        mask = np.random.rand(h, w) < 0.1
        points, dist = bfs(mask, start, stop)
        
        if points:
            break
        
        print("no path exists")
    
    print("found map with existing map")
    
    x, y = np.array(points).T
    plt.plot(x, y)
    
    result = (dist, points, mask)
    
    for _ in range(10000):
        mask2 = mask.copy()
        for _ in range(40):
            x = np.random.randint(w)
            y = np.random.randint(h)
            mask2[y, x] = 1
        
        points2, dist2 = bfs(mask2, start, stop)
        
        if points2:
            if dist2 > result[0]:
                print(dist2)
                result = (dist2, points2, mask2)
    
    dist2, points2, mask2 = result
    
    mask3 = np.zeros((h, w, 3))
    for y in range(h):
        for x in range(w):
            if mask[y, x]:
                mask3[y, x] = [1, 1, 1]
            elif mask2[y, x]:
                mask3[y, x] = [1, 0, 0]
    plt.imshow(mask3, interpolation='nearest')
    
    x, y = np.array(points).T
    plt.plot(x, y)
    
    x, y = np.array(points2).T
    plt.plot(x, y)
    
    plt.show()

main()
