var nodeRadius = 25

function Graph(){
    this.nodes     = []
    this.pos       = []
    this.neighbors = []
}

function makeNode(G, x, y){
    var node = 0
    while (contains(G.nodes, node)) node++
    
    G.nodes.push(node)
    
    while (G.pos.length <= node) G.pos.push(null)
    while (G.neighbors.length <= node) G.neighbors.push(null)
    
    G.pos[node] = [x, y]
    G.neighbors[node] = []
    
    return node
}

function hasEdge(G, a, b){
    return contains(G.neighbors[a], b)
}

function makeEdge(G, a, b){
    pushUnique(G.neighbors[a], b)
    pushUnique(G.neighbors[b], a)
}

function removeNode(G, node){
    G.pos[node] = null
    G.neighbors[node] = null
    G.nodes = remove(G.nodes, node)
    forEach(G.nodes, function(temp){
        G.neighbors[temp] = remove(G.neighbors[temp], node)
    })
}

function removeEdge(G, a, b){
    G.neighbors[a] = remove(G.neighbors[a], b)
    G.neighbors[b] = remove(G.neighbors[b], a)
}

function drawNode(G, node, backgroundColor){
    if (backgroundColor === undefined){
        backgroundColor = "#fff"
    }
    
    context.fillStyle = backgroundColor
    var [x, y] = G.pos[node]
    fillCircle(x, y, nodeRadius)
    
    context.strokeStyle = "black"
    strokeCircle(x, y, nodeRadius)
    
    context.fillStyle = "black"
    var text = node + ""
    var width = context.measureText(text).width
    context.fillText(text, x - 0.5*width, y + fontHeight*0.35)
}

function drawNodes(G){
    forEach(G.nodes, node => drawNode(G, node))
}

function drawEdges(G){
    forEach(G.nodes, function(node){
        forEach(G.neighbors[node], function(neighbor){
            var [ax, ay] = G.pos[node]
            var [bx, by] = G.pos[neighbor]
            strokeLine(ax, ay, bx, by)
        })
    })
}

function drawGraph(G){
    drawEdges(G)
    drawNodes(G)
}

function makeStar(xCenter, yCenter, radius, n){
    var G = new Graph()
    var center = makeNode(G, xCenter, yCenter)
    for (var i = 0; i < n; i++){
        var angle = 2*Math.PI/n*i
        var x = xCenter + radius*Math.cos(angle)
        var y = yCenter + radius*Math.sin(angle)
        var leaf = makeNode(G, x, y)
        makeEdge(G, center, leaf)
    }
    return G
}

function Matrix(m, n, values){
    this.values = values
    this.m = m
    this.n = n
}

Matrix.prototype.set = function(i, j, value){
    this.values[i*this.n + j] = value
}

Matrix.prototype.get = function(i, j){
    return this.values[i*this.n + j]
}

function shortestPaths(G){
    var n = max(G.nodes) + 1
    var dist = new Matrix(n, n, new Float64Array(n*n))
    var next = new Matrix(n, n, new Int32Array(n*n))
    
    for (var i = 0; i < n*n; i++){
        dist.values[i] = Number.POSITIVE_INFINITY
        next.values[i] = -1
    }
    
    forEach(G.nodes, function(node){
        forEach(G.neighbors[node], function(neighbor){
            dist.set(node, neighbor, 1)
            next.set(node, neighbor, neighbor)
        })
    })
    
    forEach(G.nodes, function(node){
        dist.set(node, node, 0)
    })
    
    // for each node
    forEach(G.nodes, function(x){
        // for each pair of nodes
        forEach(G.nodes, function(a){
            forEach(G.nodes, function(b){
                var newDist = dist.get(a, x) + dist.get(x, b)
                
                if (dist.get(a, b) > newDist){
                    dist.set(a, b, newDist)
                    next.set(a, b, next.get(a, x))
                }
            })
        })
    })
    
    return [dist, next]
}
/*
function shortestPath(G, start, goal){
    var n = G.nodes.length
    var visited = new Uint8Array(n)
    var front = [start]
    var previous = new Int32Array(n)
    for (var i = 0; i < n; i++){
        previous[i] = -1
    }
    visited[start] = 1
    
    for (var i = 0; i < front.length; i++){
        var node = front[i]
        
        if (node == goal){
            var path = []
            while (1){
                path.push(node)
                node = previous[node]
                if (node == -1) break
            }
            return reversed(path)
        }
        
        forEach(G.neighbors[node], function(neighbor){
            if (!visited[neighbor]){
                visited[neighbor] = 1
                previous[neighbor] = node
                front.push(neighbor)
            }
        })
    }
    
    return []
}

*/
