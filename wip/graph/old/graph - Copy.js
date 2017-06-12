var nodeRadius = 25
var nextNodeId = 0

function Node(x, y, id){
    if (id === undefined){
        id = nextNodeId
        nextNodeId++
    }
    
    this.pos = [x, y]
    this.id = id
}

function Graph(){
    this.nodes     = []
    this.edges     = []
}

Graph.prototype.node = function(x, y){
    var node = new Node(x, y)
    this.nodes.push(node)
    return node
}

Graph.prototype.edge = function(a, b){
    this.edges.push([a, b])
    this.edges.push([b, a])
}

Graph.prototype.removeNode = function(nodeToRemove){
    this.nodes = filter(this.nodes, node => node !== nodeToRemove)
    this.edges = filter(this.edges, edge => !contains(edge, nodeToRemove))
}

function drawNode(node, backgroundColor){
    if (backgroundColor === undefined){
        backgroundColor = "#fff"
    }
    
    context.fillStyle = backgroundColor
    var [x, y] = node.pos
    fillCircle(x, y, nodeRadius)
    
    context.strokeStyle = "black"
    strokeCircle(x, y, nodeRadius)
    
    context.fillStyle = "black"
    var text = node.id + ""
    var width = context.measureText(text).width
    context.fillText(text, x - 0.5*width, y + fontHeight*0.35)
}

function drawNodes(G){  
    forEach(G.nodes, drawNode)
}

function drawEdge(edge){
    var [ax, ay] = edge[0].pos
    var [bx, by] = edge[1].pos
    strokeLine(ax, ay, bx, by)
}

function drawEdges(G){
    context.strokeStyle = "black"
    forEach(G.edges, drawEdge)
}

function drawGraph(G){
    drawEdges(G)
    drawNodes(G)
}

function makeStar(xCenter, yCenter, radius, n){
    var G = new Graph()
    var center = G.node(xCenter, yCenter)
    for (var i = 0; i < n; i++){
        var angle = 2*Math.PI/n*i
        var x = xCenter + radius*Math.cos(angle)
        var y = yCenter + radius*Math.sin(angle)
        var leaf = G.node(x, y)
        G.edge(center, leaf)
    }
    return G
}

// will fail for more than 65536 nodes
function key(a, b){
    return a.id + (b.id << 16)
}

function shortestPaths(G){
    var d = {}
    
    forEach(G.nodes, function(node){
        d[key(node, node)] = 0
    })
    
    forEach(G.edges, function(edge){
        var [a, b] = edge
        d[key(a, b)] = 1
    })
    
    
    
    forEach(G.nodes, function(between){
        forEach(G.nodes, function(a){
            forEach(G.nodes, function(b){
                var da = d[key(a, between)]
                var db = d[key(between, b)]
                if (da !== undefined && db !== undefined){
                    var dab = d[key(a, b)]
                    if (dab){
                        d[key(a, b)] = Math.min(dab, da + db)
                    }else{
                        d[key(a, b)] = da + db
                    }
                }
            })
        })
    })
    
    return d
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
