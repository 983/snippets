//var G = makeStar(width/2, height/2, 200.0, 10)
//var G = new Graph()
var G = JSON.parse('{"nodes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22],"pos":{"0":[50,273],"1":[145,271],"2":[257,273],"3":[385,263],"4":[520,235],"5":[676,100],"6":[818,90],"7":[511,595],"8":[727,606],"9":[934,175],"10":[965,359],"11":[794,380],"12":[695,288],"13":[597,305],"14":[554,392],"15":[172,583],"16":[155,387],"17":[315,402],"18":[424,398],"19":[206,127],"20":[350,122],"21":[532,84],"22":[524,553]},"neighbors":{"0":{"1":1,"16":1,"19":1},"1":{"0":1,"2":1,"16":1,"19":1},"2":{"1":1,"3":1,"16":1,"17":1,"19":1},"3":{"2":1,"4":1,"17":1},"4":{"3":1,"5":1,"17":1,"18":1,"20":1,"21":1},"5":{"4":1,"6":1,"21":1},"6":{"5":1,"9":1},"7":{},"8":{},"9":{"6":1,"10":1,"11":1},"10":{"9":1,"11":1},"11":{"9":1,"10":1,"12":1,"14":1},"12":{"11":1,"13":1},"13":{"12":1,"14":1},"14":{"11":1,"13":1,"18":1},"15":{},"16":{"0":1,"1":1,"2":1,"17":1},"17":{"2":1,"3":1,"4":1,"16":1,"18":1},"18":{"4":1,"14":1,"17":1},"19":{"0":1,"1":1,"2":1,"20":1},"20":{"4":1,"19":1,"21":1},"21":{"4":1,"5":1,"20":1},"22":{}}}')

//var G = makeRandomDiskGraph(30, 150)
/*
function makeRandomDiskGraph(n, radius){
    var G = new Graph()
    for (var i = 0; i < n; i++){
        var x = Math.random()*(width - 2*nodeRadius) + nodeRadius
        var y = Math.random()*(height - 2*nodeRadius) + nodeRadius
        G.node(x, y)
    }
    for (var i = 0; i < n; i++){
        for (var j = i + 1; j < n; j++){
            if (distance(G.pos[i], G.pos[j]) < radius){
                G.edge(i, j)
                G.edge(j, i)
            }
        }
    }
    return G
}

function drawPath(G, path){
    for (var i = 0; i + 1 < path.length; i++){
        var a = path[i]
        var b = path[i + 1]
        
        var [ax, ay] = G.pos[a]
        var [bx, by] = G.pos[b]
        
        strokeArrow(ax, ay, bx, by)
    }
}
*/

var [d, next] = shortestPaths(G)

function numberToWord(value){
    if (value === Number.POSITIVE_INFINITY) return "∞"
    if (value === Number.NEGATIVE_INFINITY) return "-∞"
    return value + ""
}

function redraw(){
    context.fillStyle = "white";
    context.fillRect(0, 0, width, height);
    
    context.strokeStyle = "#000"
    drawEdges(G)
    context.strokeStyle = "#f00"
    
    context.lineWidth = 3
    /*
    if (selected != -1){
        var path = shortestPath(G, 0, selected)
        drawPath(G, path)
    }
    */
    
    var [d, next] = shortestPaths(G)
    
    var target = 6
    
    var beacons = [10, 14, 20, 16]
    
    // ambiguous in case of equal distances
    //beacons = sorted(beacons, beacon => d[key(beacon, target)])
    
    var addresses = {}
    
    if (selected !== null){
        context.strokeStyle = "#0ff"
        var node = selected
        while (1){
            var [ax, ay] = G.pos[node]
            node = next[key(node, target)]
            
            if (node === target){
                console.log("path exists")
                break
            }
            
            if (node === undefined){
                console.log("path does not exist")
                break
            }
            
            var [bx, by] = G.pos[node]
            
            strokeArrow(ax, ay, bx, by)
        }
    }
    
    context.fillStyle = "black"
    forEach(G.nodes, function(node){
        var text = []
        forEach(beacons, function(beacon){
            var dist = d[key(node, beacon)]
            text.push(numberToWord(dist))
        })
        
        text = "(" + text.join(", ") + ")"
        
        var [x, y] = G.pos[node]
        
        var width = context.measureText(text).width
        context.fillText(text, x - 0.5*width, y - nodeRadius - fontHeight*0.2)
        
        var address = []
        var s = 0
        forEach(beacons, function(beacon){
            s += Math.abs(d[key(node, beacon)] - d[key(beacon, target)])
            address.push(numberToWord(s))
        })
        address = reversed(address)
        
        addresses[node] = address
        
        text = "(" + address.join(", ") + ")"
        
        var width = context.measureText(text).width
        context.fillText(text, x - 0.5*width, y + nodeRadius + fontHeight*0.8)
    })
    
    {
        var start = 0
        var node = start
        var minAddress = addresses[node]
        
        for (var k = 0; k < 5; k++){
            var currentAddress = addresses[node]
            for (var i = 0; i < minAddress.length; i++){
                minAddress[i] = Math.min(minAddress[i], currentAddress[i])
            }
            
            var couldDoGreedyStep = false
            
            for (var i = minAddress.length - 1; i >= 0; i--){
                var bestNode = node
                for (var neighbor in G.neighbors[node]){
                    var neighborAddress = addresses[neighbor]
                    if (neighborAddress[i] < minAddress[i] &&
                        neighborAddress[i] < addresses[bestNode][i]
                    ){
                        bestNode = neighbor
                    }
                }
                
                if (bestNode != node){
                    couldDoGreedyStep = true
                    
                    var [ax, ay] = G.pos[node]
                    var [bx, by] = G.pos[bestNode]
                    context.strokeStyle = "#f00"
                    strokeArrow(ax, ay, bx, by)
                    node = bestNode
                    break
                }
            }
            
            if (!couldDoGreedyStep){
                /*
                var closestBeacon = beacons[0]
                var [ax, ay] = G.pos[node]
                var [bx, by] = G.pos[next[key(node, closestBeacon)]]
                context.strokeStyle = "blue"
                strokeArrow(ax, ay, bx, by)
                */
                /*
                
                var path = findPath(G, node, closestBeacon)
                
                context.strokeStyle = "blue"
                for (var i = 0; i < path.length - 1; i++){
                    strokeArrow(ax, ay, bx, by)
                }
                */
            }
        }
    }
    
    context.lineWidth = 1
    
    drawNodes(G)
    
    forEach(beacons, function(beacon){
        drawNode(G, beacon, "lightblue")
    })
    
    if (selected !== null){
        drawNode(G, selected, "#0ff")
    }
}

var mouseOld = [0, 0]
var mouse = [0, 0]
var mouseDown = {}

var selected = null

function moveMouse(e){
    var rect = canvas.getBoundingClientRect()
    var x = e.clientX - rect.left
    var y = e.clientY - rect.top
    
    var dx = x - mouse[0]
    var dy = y - mouse[1]
    
    mouseOld = mouse
    mouse = [x, y]
    
    if (selected !== null && mouseDown[1]){
        var p = G.pos[selected]
        p[0] += dx
        p[1] += dy
    }
}

window.onmousedown = function(e){
    moveMouse(e)
    
    mouseDown[e.which] = true
    
    if (e.which == 1){
        selected = null
        forEach(G.nodes, function(node){
            if (distance(G.pos[node], mouse) < nodeRadius){
                selected = node
            }
        })
        if (selected === null){
            var [x, y] = mouse
            var node = makeNode(G, x, y)
            selected = node
        }
    }
    
    if (e.which == 3){
        if (selected !== null){
            forEach(G.nodes, function(node){
                if (distance(G.pos[node], mouse) < nodeRadius){
                    if (G.neighbors[node][selected] === undefined){
                        makeEdge(G, node, selected)
                    }else{
                        removeEdge(G, node, selected)
                    }
                }
            })
        }
    }
    
    redraw()
}

canvas.oncontextmenu = function(e){
    e.preventDefault()
}

window.onmouseup = function(e){
    moveMouse(e)
    
    mouseDown[e.which] = false
    
    if (e.which == 1){
    }
    
    redraw()
}

window.onmousemove = function(e){
    moveMouse(e)
    redraw()
}

window.onkeydown = function(e){
    //console.log(e)
    var KEY_DELETE = 46
    if (e.keyCode == KEY_DELETE){
        if (selected !== null){
            removeNode(G, selected)
            selected = null
        }
    }
    redraw()
}

redraw()
