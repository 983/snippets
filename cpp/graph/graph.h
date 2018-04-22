#include "util.h"

typedef size_t Vertex;

struct Edge {
    Vertex a, b;
};

typedef Vector<Edge> Edges;
typedef Vector<Vertex> Vertices;

Edge flip(const Edge &edge){
    return Edge{edge.b, edge.a};
}

bool operator == (const Edge &ab, const Edge &cd){
    return ab.a == cd.a && ab.b == cd.b;
}

Points get_intersections(const Edges &edges, const Points &points){
    Points intersections;
    
    for (size_t i = 0; i < edges.size(); i++){
        Edge ab = edges[i];
        
        Vertex a = ab.a;
        Vertex b = ab.b;
        
        Point pa = points[a];
        Point pb = points[b];
        
        for (size_t j = i + 1; j < edges.size(); j++){
            Edge cd = edges[j];
            
            Vertex c = cd.a;
            Vertex d = cd.b;
            
            Point pc = points[c];
            Point pd = points[d];
            
            if (a == c || a == d || b == c || b == d) continue;
            
            Point temp[4];
            
            int n = intersect(pa, pb, pc, pd, temp);
            
            for (int k = 0; k < n; k++){
                intersections.push_back(temp[k]);
            }
        }
    }
    
    return intersections;
}

double min_vertex_edge_dist2(
    const Vertices &vertices,
    const Edges &edges,
    const Points &points
){
    double min_dist2 = INFINITY;
    
    for (const Edge &edge : edges){
        Vertex a = edge.a;
        Vertex b = edge.b;
        
        Point pa = points[a];
        Point pb = points[b];
        
        for (const Vertex &vertex : vertices){
            if (vertex == a || vertex == b) continue;
            
            Point p = points[vertex];
            
            double dist2 = point_line_dist2(p, pa, pb);
            
            min_dist2 = std::min(min_dist2, dist2);
        }
    }
    
    return min_dist2;
}
