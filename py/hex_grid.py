import os, sys, time
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"
import pygame
import numpy as np
import numpy.linalg
import scipy.spatial
import scipy.sparse
from OpenGL.GL import *

BLACK = (0, 0, 0, 1)
BLUE = (0, 0, 1, 1)
LIGHT_GRAY = (0.8, 0.8, 0.8, 1)

def make_grid_graph(w, h):
    neighborhood = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    #neighborhood.extend([(-1, -1), (-1, 1), (1, 1), (1, -1)])
    n = h * w
    vertices = np.arange(n)
    grid = vertices.reshape(h, w)
    x = np.tile(np.arange(w), h)
    y = np.repeat(np.arange(h), w)
    points = np.stack([x, y], axis=1)
    edges = []
    
    for y in range(h):
        for x in range(w):
            i = grid[y][x]
            
            for dx, dy in neighborhood:
                x2 = x + dx
                y2 = y + dy
                if 0 <= x2 < w and 0 <= y2 < h:
                    j = grid[y2][x2]
                    
                    edges.append((i, j))
                    edges.append((j, i))

    return vertices, edges, points

def knn(data_points, query_points, k):
    tree = scipy.spatial.cKDTree(data_points)
    return tree.query(query_points, k=k)

def delaunay_edges(points):
    n = len(points)
    tri = scipy.spatial.Delaunay(points)
    indptr, indices = tri.vertex_neighbor_vertices
    edges = [(i, j)
        for i in range(n)
        for j in indices[indptr[i]:indptr[i+1]]]
    return edges

def make_delaunay_graph(n):
    vertices = np.arange(n)
    points = np.random.rand(n, 2) * np.float64([[32, 24]])
    edges = delaunay_edges(points)
    return vertices, edges, points

def make_hex_graph(w, h):
    n = h * w
    vertices = np.arange(n)
    grid = vertices.reshape(h, w)
    x = np.arange(w).astype(np.float64)
    y = np.arange(h).astype(np.float64)
    x, y = np.meshgrid(x, y)
    x[1::2] += 0.5
    x = x.flatten()
    y = y.flatten()
    points = np.stack([x, y], axis=1)
    
    _, neighbors = knn(points, points, 10)
    
    edges = [
        (i, j)
        for i in vertices
        for j in neighbors[i]
        if np.linalg.norm(points[i] - points[j]) <= 1.7]
    
    return vertices, edges, points

class Mesh:
    def __init__(self, max_vertices=10**6):
        self.points = np.zeros((max_vertices, 2), dtype=np.float32)
        self.colors = np.zeros((max_vertices, 4), dtype=np.float32)
        self.n = 0
    
    def draw(self, mode=GL_TRIANGLES):
        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        glVertexPointer(2, GL_FLOAT, 0, self.points[:self.n])
        glColorPointer(4, GL_FLOAT, 0, self.colors[:self.n])
        glDrawArrays(mode, 0, self.n)

    def circle(self, c, radius, color, n=16):
        cx, cy = c
        angle = 2 * np.pi / n * np.arange(n)
        x = cx + radius * np.cos(angle)
        y = cy + radius * np.sin(angle)
        points = np.stack([x, y], axis=1)
        a = points[-1]
        n = self.n
        for b in points:
            self.points[n + 0] = c
            self.points[n + 1] = a
            self.points[n + 2] = b
            for _ in range(3):
                self.colors[n] = color
                n += 1
            a = b
        self.n = n
    
    def line(self, a, b, radius, color):
        ax, ay = a
        bx, by = b
        dx = bx - ax
        dy = by - ay
        
        d2 = dx * dx + dy * dy
        
        if d2 == 0: return
        
        d = np.sqrt(d2)
        
        vx = radius / d * dx
        vy = radius / d * dy
        
        p0 = (ax + vy, ay - vx)
        p1 = (bx + vy, by - vx)
        p2 = (bx - vy, by + vx)
        p3 = (ax - vy, ay + vx)
        
        n = self.n
        self.points[n:n+6] = [p0, p1, p2, p0, p2, p3]
        self.colors[n:n+6] = color
        self.n = n + 6

def main():
    np.random.seed(0)
    pygame.font.init()
    pygame.display.init()

    size = (640, 480)
    
    background_color = (1.0, 1.0, 1.0, 1.0)

    flags = pygame.DOUBLEBUF | pygame.RESIZABLE | pygame.OPENGL

    pygame.display.gl_set_attribute(pygame.GL_MULTISAMPLEBUFFERS, 1)
    pygame.display.gl_set_attribute(pygame.GL_MULTISAMPLESAMPLES, 16)
    
    screen = pygame.display.set_mode(size, flags=flags)
    
    running = True
    
    font = pygame.font.SysFont('Arial', 25)
    
    glEnable(GL_MULTISAMPLE)
    print("GL_SAMPLE_BUFFERS:", glGetIntegerv(GL_SAMPLE_BUFFERS))
    print("GL_SAMPLES:", glGetIntegerv(GL_SAMPLES))
    
    #graph = make_grid_graph(32, 24)
    #graph = make_delaunay_graph(500)
    graph = make_hex_graph(32, 24)
    
    vertices, edges, points = graph
    
    n = len(vertices)
    
    blocker = [n // 2 + 25]
    
    for _ in range(3):
        blocker = [j for i, j in edges if i in blocker]
    
    print(blocker)
    
    edges = [(i, j) for i, j in edges if i not in blocker and j not in blocker]
    
    edges = np.array(edges)
    
    i, j = edges.T
    
    #edge_weights = np.linalg.norm(points[i] - points[j], axis=1)
    edge_weights = np.ones(len(edges))
    
    distances0 = scipy.sparse.csr_matrix((edge_weights, (i, j)), shape=(n, n))

    distances, prev = scipy.sparse.csgraph.shortest_path(distances0, return_predecessors=True)
    
    next = prev.T
    
    def write(text, p, text_color=(0, 0, 0, 255)):
        pixels = font.render(text, True, text_color)
        data = pygame.image.tostring(pixels, "RGBA", True)
        glColor3f(1, 1, 1)
        glRasterPos2f(*p)
        glDrawPixels(pixels.get_width(), pixels.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, data)
    
    mesh = Mesh()
    
    for i, j in edges:
        mesh.line(points[i], points[j], 0.02, LIGHT_GRAY)
    
    for i in vertices:
        mesh.circle(points[i], 0.15, LIGHT_GRAY)

    # find bottom-left vertex
    i = np.argmin(points.sum(axis=1))
    # find furthest vertex
    j = np.argmax(np.linalg.norm(points - points[i], axis=1))
    
    while i != j:
        i_next = next[i, j]
        
        mesh.line(points[i], points[i_next], 0.05, BLUE)
        
        i = i_next

    for i in blocker:
        mesh.circle(points[i], 0.15, BLACK)

    while running:
        keys = pygame.key.get_pressed()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                break
            
            elif event.type == pygame.KEYDOWN:
                if event.key in [pygame.K_q, pygame.K_ESCAPE]:
                    print("quit")
                    running = False
                    break
        
            elif event.type == pygame.VIDEORESIZE:
                size = event.size
                screen = pygame.display.set_mode(size, flags=flags)
        
        glViewport(0, 0, size[0], size[1])
        glClearColor(*background_color)
        glClear(GL_COLOR_BUFFER_BIT)
        
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        x0, y0 = points.min(axis=0)
        x1, y1 = points.max(axis=0)
        pad = 0.5
        glOrtho(x0 - pad, x1 + pad, y0 - pad, y1 + pad, -1, 1)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        
        mesh.draw()
        
        pygame.display.flip()

        time.sleep(0.03)
    
    pygame.quit()
    sys.exit(0)

if __name__ == "__main__":
    main()
