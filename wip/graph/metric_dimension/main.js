"use strict"

var G = new Graph()
// homework graph
var G = JSON.parse('{"nodes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],"pos":[[185,70],[296,45],[291,123],[209,213],[163,304],[254,339],[288,254],[394,292],[365,384],[564,230],[530,120],[662,190],[627,332],[749,305],[774,384],[663,432]],"neighbors":[[1,2],[0,2],[0,1,3,10,7,6],[2,4,5,6],[3,5,6],[3,4,6,8,7],[3,4,5,8,7,2],[8,5,6,10,2,9],[6,5,7,9,12],[10,7,8,12,11],[9,11,2,7],[10,9,12],[9,8,13,14,15,11],[12,15,14],[12,15,13],[12,14,13]]}')

function numberToString(value){
    if (value === Number.POSITIVE_INFINITY) return "∞"
    if (value === Number.NEGATIVE_INFINITY) return "-∞"
    return value + ""
}

function drawNodeText(G, texts){
    var k = 0
    forEach(G.nodes, function(node){
        var text = texts[k++]
        var [x, y] = G.pos[node]
        var width = context.measureText(text).width
        context.fillText(text, x - 0.5*width, y + nodeRadius - fontHeight*2.8)
    })
}

function makeAddresses(G, dist, resolvingSet){
    var addresses = []
    
    forEach(G.nodes, function(node){
        var address = []
        forEach(resolvingSet, function(anchor){
            address.push(dist.get(node, anchor))
        })
        
        var text = "(" + map(address, numberToString).join(", ") + ")"
        
        addresses.push(text)
    })
    
    return addresses
}

function isResolvingSet(G, dist, resolvingSet){
    var addresses = makeAddresses(G, dist, resolvingSet)
    
    return unique(addresses).length == addresses.length
}

var counter = 0

function doMyHomework(){
    var [dist, next] = shortestPaths(G)

    var resolvingSets = []

    for (var subset of subsets(G.nodes, 0, 4)) {
        if (isResolvingSet(G, dist, subset)){
            resolvingSets.push(copy(subset))
        }
    }
    
    console.log(resolvingSets.length, "resolving sets")
    
    if (resolvingSets.length == 0) return
    
    var resolvingSet = resolvingSets[counter % resolvingSets.length]
    counter++
    
    var addresses = makeAddresses(G, dist, resolvingSet)
    
    forEach(resolvingSet, function(node){
        drawNode(G, node, "#0f0")
    })
    
    drawNodeText(G, addresses)
}

function redraw(){
    context.fillStyle = "#fff"
    context.fillRect(0, 0, width, height)
    
    context.strokeStyle = "#000"
    drawEdges(G)
    drawNodes(G)
    
    context.strokeStyle = "#000"
    
    doMyHomework()
    
    context.lineWidth = 1
    
    if (selected !== -1){
        drawNode(G, selected, "#0ff")
    }
}

var mouseOld = [0, 0]
var mouse = [0, 0]
var mouseDown = {}

var selected = -1

function moveMouse(e){
    var rect = canvas.getBoundingClientRect()
    var x = e.clientX - rect.left
    var y = e.clientY - rect.top
    
    var dx = x - mouse[0]
    var dy = y - mouse[1]
    
    mouseOld = mouse
    mouse = [x, y]
    
    if (selected !== -1 && mouseDown[1]){
        var p = G.pos[selected]
        p[0] += dx
        p[1] += dy
    }
}

window.onmousedown = function(e){
    moveMouse(e)
    
    mouseDown[e.which] = true
    
    if (e.which == 1){
        selected = -1
        forEach(G.nodes, function(node){
            if (distance(G.pos[node], mouse) < nodeRadius){
                selected = node
            }
        })
        if (selected === -1){
            var [x, y] = mouse
            var node = makeNode(G, x, y)
            selected = node
        }
    }
    
    if (e.which == 3){
        if (selected !== -1){
            forEach(G.nodes, function(node){
                if (distance(G.pos[node], mouse) < nodeRadius){
                    if (hasEdge(G, node, selected)){
                        removeEdge(G, node, selected)
                    }else{
                        makeEdge(G, node, selected)
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
        if (selected !== -1){
            removeNode(G, selected)
            selected = -1
        }
    }
    redraw()
}

redraw()
