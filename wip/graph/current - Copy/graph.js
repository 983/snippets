var nodeRadius = 25

function Graph(){
    this.nodes     = []
    this.pos       = {}
    this.neighbors = {}
}

function makeNode(G, x, y){
    var node = 0
    while (contains(G.nodes, node)) node++
    
    G.nodes.push(node)
    G.pos[node] = [x, y]
    G.neighbors[node] = {}
    return node
}

function makeEdge(G, a, b){
    G.neighbors[a][b] = 1
    G.neighbors[b][a] = 1
}

function removeNode(G, node){
    // delete position of node
    delete G.pos[node]
    
    // delete edges from neighbors of node to node
    for (var neighbor in G.neighbors[node]){
        delete G.neighbors[neighbor][node]
    }
    
    // delete edges from node to neighbors
    delete G.neighbors[node]
    
    // delete node
    G.nodes = filter(G.nodes, a => a !== node)
}

function removeEdge(G, a, b){
    delete G.neighbors[a][b]
    delete G.neighbors[b][a]
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
        for (var neighbor in G.neighbors[node]){
            var [ax, ay] = G.pos[node]
            var [bx, by] = G.pos[neighbor]
            strokeLine(ax, ay, bx, by)
        }
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

// will fail for more than 65536 nodes
function key(a, b){
    return a + (b << 16)
}

function shortestPaths(G){
    var d = {}
    var next = {}
    
    forEach(G.nodes, function(a){
        forEach(G.nodes, function(b){
            d[key(a, b)] = Number.POSITIVE_INFINITY
        })
    })
    
    forEach(G.nodes, function(node){
        for (var neighbor in G.neighbors[node]){
            d[key(node, neighbor)] = 1
            
            next[key(node, neighbor)] = neighbor
        }
    })
    
    forEach(G.nodes, function(node){
        d[key(node, node)] = 0
    })
    
    // for each node
    forEach(G.nodes, function(between){
        // for each pair of nodes
        forEach(G.nodes, function(a){
            forEach(G.nodes, function(b){
                var da = d[key(a, between)]
                var db = d[key(between, b)]
                
                var dab = d[key(a, b)]
                
                d[key(a, b)] = Math.min(da + db, dab)
                /*
                if (da + db < dab){
                    d[key(a, b)] = da + db
                    //next[key(a, b)] = next[key(a, between)]
                }
                */
                /*
                // if path(a, between) and path(between, b) exist
                if (da !== undefined && db !== undefined){
                    
                    var dab = d[key(a, b)]
                    
                    // if path(a, b) exists
                    if (dab !== undefined){
                        // if path(a, between) + path(between, b) is shorter than path(a, b)
                        if (da + db < dab){
                            next[key(a, b)] = next[key(a, between)]
                            d[key(a, b)] = da + db
                        }
                    }else{
                        // else path(a, b) does not exist yet and we found one over 'between'
                        next[key(a, b)] = next[key(a, between)]
                        d[key(a, b)] = da + db
                    }
                }
                */
            })
        })
    })
    
    return [d, next]
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
