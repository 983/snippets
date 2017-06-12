"use strict"
//var G = makeStar(width/2, height/2, 200.0, 10)

/*
// example from script
var G = JSON.parse('{"nodes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21],"pos":{"0":[50,273],"1":[145,271],"2":[257,273],"3":[385,263],"4":[520,204],"5":[676,100],"6":[818,90],"7":[511,595],"8":[727,606],"9":[934,175],"10":[939,353],"11":[794,380],"12":[717,265],"13":[598,300],"14":[554,392],"15":[172,583],"16":[146,415],"17":[313,413],"18":[430,395],"19":[206,127],"20":[350,122],"21":[532,84]},"neighbors":{"0":{"1":1,"16":1,"19":1},"1":{"0":1,"2":1,"16":1,"19":1},"2":{"1":1,"3":1,"16":1,"17":1,"19":1},"3":{"2":1,"4":1,"17":1},"4":{"3":1,"5":1,"17":1,"18":1,"20":1,"21":1},"5":{"4":1,"6":1,"21":1},"6":{"5":1,"9":1},"7":{},"8":{},"9":{"6":1,"10":1,"11":1},"10":{"9":1,"11":1},"11":{"9":1,"10":1,"12":1,"14":1},"12":{"11":1,"13":1},"13":{"12":1,"14":1},"14":{"11":1,"13":1,"18":1},"15":{},"16":{"0":1,"1":1,"2":1,"17":1},"17":{"2":1,"3":1,"4":1,"16":1,"18":1},"18":{"4":1,"14":1,"17":1},"19":{"0":1,"1":1,"2":1,"20":1},"20":{"4":1,"19":1,"21":1},"21":{"4":1,"5":1,"20":1}}}')
var beacons = [20, 16, 14, 10]
// ordered by distance to target
var orderedBeacons = [10, 14, 20, 16]
var start = 0
var target = 6
*/
// homework graph
var G = JSON.parse('{"nodes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20],"pos":{"0":[53,213],"1":[184,75],"2":[169,211],"3":[71,352],"4":[227,377],"5":[289,260],"6":[310,127],"7":[436,101],"8":[430,220],"9":[411,340],"10":[382,452],"11":[532,457],"12":[630,361],"13":[611,252],"14":[567,127],"15":[719,196],"16":[834,177],"17":[816,309],"18":[775,431],"19":[957,402],"20":[972,230]},"neighbors":{"0":{"1":1,"2":1,"3":1},"1":{"0":1,"2":1,"5":1,"6":1},"2":{"0":1,"1":1,"3":1,"4":1},"3":{"0":1,"2":1,"4":1},"4":{"2":1,"3":1,"5":1,"9":1},"5":{"1":1,"4":1,"6":1,"8":1},"6":{"1":1,"5":1,"7":1},"7":{"6":1,"8":1},"8":{"5":1,"7":1,"9":1,"14":1},"9":{"4":1,"8":1,"10":1,"11":1,"12":1},"10":{"9":1,"11":1},"11":{"9":1,"10":1,"12":1},"12":{"9":1,"11":1,"13":1,"18":1},"13":{"12":1,"14":1},"14":{"8":1,"13":1,"15":1},"15":{"14":1,"16":1,"18":1},"16":{"15":1,"17":1,"20":1},"17":{"16":1,"18":1,"20":1},"18":{"12":1,"15":1,"17":1,"19":1},"19":{"18":1,"20":1},"20":{"16":1,"17":1,"19":1}}}')
var beacons = [9, 7, 12, 20]
// ordered by distance to target
var orderedBeacons = [20, 12, 9, 7]
var start = 0
var target = 17

//var G = new Graph()

var wayTooMuchWork = false

// misconceptions
// the thing above the nodes is the address, below is the distance
// address and distances are based on different beacon orderings
// the addresses are compared number by number, not lexicographically
// fallback mode only happens for one step
// there are too many types of distances

var [dist, next] = shortestPaths(G)

function numberToWord(value){
    if (value === Number.POSITIVE_INFINITY) return "∞"
    if (value === Number.NEGATIVE_INFINITY) return "-∞"
    return value + ""
}

function beaconRouting(beacons, orderedBeacons, start, target){
    context.strokeStyle = "#f00"
    
    context.lineWidth = 3
    
    var [dist, next] = shortestPaths(G)
    
    // ambiguous in case of equal distances
    //orderedBeacons = sorted(beacons, beacon => d[key(beacon, target)])
    
    var distances = {}
    
    context.fillStyle = "black"
    forEach(G.nodes, function(node){
        var address = []
        forEach(beacons, function(beacon){
            address.push(dist[node][beacon])
        })
        
        if (wayTooMuchWork){
            var meanSquared = 0
            for (var j = 0; j < beacons.length; j++){
                var beacon = beacons[j]
                var d = dist[node][beacon]
                meanSquared += d*d
            }
            meanSquared /= beacons.length
            
            for (var i = 0; i < beacons.length; i++){
                address[i] = address[i]*address[i] - meanSquared
            }
        }
        
        var text = []
        for (var i = 0; i < beacons.length; i++){
            text.push(numberToWord(address[i]))
        }
        text = "(" + text.join(", ") + ")"
        
        var [x, y] = G.pos[node]
        
        var width = context.measureText(text).width
        context.fillText(text, x - 0.5*width, y - nodeRadius - fontHeight*0.2)
        
        distances[node] = []
        var s = 0
        if (wayTooMuchWork){
            forEach(orderedBeacons, function(beacon){
                var difference = dist[node][beacon] - dist[beacon][target]
                s += difference*difference
                distances[node].push(numberToWord(s))
            })
        }else{
            forEach(orderedBeacons, function(beacon){
                s += Math.abs(dist[node][beacon] - dist[beacon][target])
                distances[node].push(numberToWord(s))
            })
        }
        distances[node] = reversed(distances[node])
        
        text = "(" + distances[node].join(", ") + ")"
        
        var width = context.measureText(text).width
        context.fillText(text, x - 0.5*width, y + nodeRadius + fontHeight*0.8)
    })
    
    if (G.nodes.length > 0){
        var node = start
        var minDist = distances[node]
        
        for (var k = 0; k < 10; k++){
            
            var couldDoGreedyStep = false
            
            // minimize minDist component-wise
            for (var i = 0; i < beacons.length; i++){
                minDist[i] = Math.min(minDist[i], distances[node][i])
            }
            
            // draw min distance below node
            var text = "(" + minDist.join(", ") + ")"
            var [x, y] = G.pos[node]
            var width = context.measureText(text).width
            context.fillStyle = "blue"
            context.fillText(text, x - 0.5*width, y + nodeRadius + fontHeight*1.8)
            context.fillStyle = "#000"
            
            // for each dist component
            for (var i = 0; i < beacons.length; i++){
                // find neighbor with minimum dist component
                var bestNode = node
                for (var neighbor in G.neighbors[node]){
                    var neighborDist = distances[neighbor]
                    if (neighborDist[i] < minDist[i]){
                        if (neighborDist[i] < distances[bestNode][i]){
                            bestNode = neighbor
                        }
                        // this can be removed
                        // it is only used to get same result in ambiguous script example
                        if (neighborDist[i] == distances[bestNode][i] && parseInt(bestNode) < parseInt(neighbor)){
                            bestNode = neighbor
                        }
                    }
                }
                
                // if a neighbor with minimum dist component exists
                if (bestNode != node){
                    // go to that neighbor
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
                var [ax, ay] = G.pos[node]
                var closestBeacon = orderedBeacons[0]
                // if we are at closest beacon already
                if (node === closestBeacon){
                    // TODO flood to all nodes within target distance
                    var [bx, by] = G.pos[target]
                    context.strokeStyle = "green"
                    context.fillStyle = "green"
                    context.fillText("flood", ax + nodeRadius, ay)
                    strokeArrow(ax, ay, bx, by)
                    break
                }
                
                node = next[node][closestBeacon]
                var [bx, by] = G.pos[node]
                context.strokeStyle = "lightblue"
                strokeArrow(ax, ay, bx, by)
            }
        }
    }
}

function redraw(){
    context.fillStyle = "#fff";
    context.fillRect(0, 0, width, height);
    
    context.strokeStyle = "#000"
    drawEdges(G)
    
    forEach(G.nodes, function(node){
        drawNode(G, node, "#fff")
    })
    
    beaconRouting(beacons, orderedBeacons, start, target)
    
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
