import matplotlib.pyplot as plt
import numpy as np
import itertools
import time
from collections import defaultdict

# TODO
# ensure that result can be decoded, i.e.
# the graph with edges [node, encoder] has no cycles
# works for this example anyway, but that is just luck

def draw_circle(x, y, radius):
    circle = plt.Circle((x, y), radius, fc='white', ec='black', fill=True, zorder=10)
    plt.gcf().gca().add_artist(circle)

def draw_line(ax, ay, bx, by, color='black'):
    plt.plot([ax, bx], [ay, by], color=color)

target = 0

p = [
    [4, 1],
    [0, 1],
    [1, 2],
    [3, 2],
    [1, 0],
    [3, 0],
]

n = len(p)

V = list(range(n))

costs = {
    (1, 2): 18,
    (1, 4):  6,
    (4, 5): 25,
    (3, 5): 38,
    (2, 3): 20,
    (0, 3):  4,
    (0, 5): 40,
    (3, 4):  5,
    (2, 4): 10,
}

def edge(a, b):
    return (min(a, b), max(a, b))

edges = list(costs.keys())

assert(all(a <= b for a,b in edges))

m = np.array([
    [9, np.inf, np.inf, np.inf, np.inf, np.inf],
    [5, 20, 2, 5, 5, 5],
    [4, 8, 10, 7, 6, 5],
    [13, 14, 10, 15, 12, 11],
    [6, 8, 10, 9, 11, 7],
    [15, 16, 3, 18, 17, 19],
])

def powerset(values):
    return itertools.chain.from_iterable(
        itertools.combinations(values, n) for n in range(len(values) + 1))

def find_reachable(node, neighbors, visited):
    for neighbor in neighbors[node]:
        if neighbor not in visited:
            visited.add(neighbor)
            find_reachable(neighbor, neighbors, visited)
    return visited

def edges_to_neighbors(edges):
    neighbors = defaultdict(set)
    for a,b in edges:
        neighbors[a].add(b)
        neighbors[b].add(a)
    return neighbors

cheapest_graphs = defaultdict(lambda: (float("inf"), None))

for subgraph_edges in powerset(edges):
    neighbors = edges_to_neighbors(subgraph_edges)
    nodes = neighbors.keys()

    cost = sum(costs[edge] for edge in subgraph_edges)
    
    for node in nodes:
        reachable = find_reachable(node, neighbors, set([node]))

        reachable = tuple(sorted(reachable))
        key = (node, reachable)

        cheapest_graphs[key] = min(cheapest_graphs[key], (cost, subgraph_edges))

dist = np.full((n, n), np.inf)
next = np.full((n, n), -1, dtype=np.int)

for node in V:
    dist[node, node] = 0

for (a,b), cost in costs.items():
    dist[a, b] = cost
    next[a, b] = b
    
    dist[b, a] = cost
    next[b, a] = a

for k in range(n):
    for i in range(n):
        for j in range(n):
            if dist[i, j] > dist[i, k] + dist[k, j]:
                dist[i, j] = dist[i, k] + dist[k, j]
                next[i, j] = next[i, k]

smallest_cost = np.inf

for encoders in itertools.product(V, repeat=n):
    d = defaultdict(list)

    node_to_target_cost = 0
    
    for node, encoder in zip(V, encoders):
        d[encoder].append(node)

        node_to_target_cost += dist[node, target]*m[node, encoder]

    encoder_to_node_cost = 0

    encoder_to_node_costs = []

    for encoder, reachable in d.items():
        reachable = tuple(sorted(reachable))
        key = (encoder, reachable)

        cost, subgraph_edges = cheapest_graphs[key]

        encoder_to_node_costs.append((cost, m[encoder, encoder]))

        encoder_to_node_cost += cost*m[encoder, encoder]

    total_cost = node_to_target_cost + encoder_to_node_cost

    if smallest_cost > total_cost:
        smallest_cost = total_cost
        print("cost:")
        print(smallest_cost)
        print("send to nodes for self-coding:")
        for (encoder, reachable), cost in zip(d.items(), encoder_to_node_costs):
            print("send", encoder, "to", reachable, "for", cost[0], "*", cost[1])
        print("now send to target:")
        for node, encoder in zip(V, encoders):
            print("send", node, "encoded with", encoder, "to", target, "for", dist[node, target], "*", m[node, encoder], "=", dist[node, target]*m[node, encoder])
        print("\n")

for (a,b), c in costs.items():
    ax, ay = p[a]
    bx, by = p[b]
    mx = (ax + bx)*0.5
    my = (ay + by)*0.5
    draw_line(ax, ay, bx, by)
    plt.text(mx, my, str(c), ha='center', va='center', bbox={'boxstyle':'round', 'facecolor':'white', 'pad':0.5})

for i, (x,y) in enumerate(p):
    draw_circle(x, y, 0.11)
    #draw_circle(x, y, 0.1, 'white')
    plt.text(x, y, str(i), zorder=11, ha='center', va='center')

plt.axes().set_aspect('equal', 'datalim')
plt.grid(True)
plt.show()
