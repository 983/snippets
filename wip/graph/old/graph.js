"use strict";
var canvas = document.getElementById("canvas")
var context = canvas.getContext("2d")
var width = canvas.width
var height = canvas.height

function strokeLine(ax, ay, bx, by){
    context.beginPath()
    context.moveTo(ax, ay)
    context.lineTo(bx, by)
    context.stroke()
}

function strokeCircle(x, y, r){
    context.beginPath()
    context.arc(x, y, r, 0, 2*Math.PI)
    context.stroke()
}

function fillCircle(x, y, r){
    context.beginPath()
    context.arc(x, y, r, 0, 2*Math.PI)
    context.fill()
}

function distance(a, b){
	var [ax, ay] = a
	var [bx, by] = b
    var dx = ax - bx
    var dy = ay - by
    return Math.sqrt(dx*dx + dy*dy)
}

function forEach(values, f){
	for (var i = 0; i < values.length; i++){
		f(values[i])
	}
}

function Graph(){
	this.nodes     = []
	this.neighbors = []
	this.pos       = []
}

Graph.prototype.node = function(x, y){
	var node = this.nodes.length
	this.pos.push([x, y])
	this.nodes.push(node)
	this.neighbors.push([])
	return node
}

Graph.prototype.edge = function(a, b){
	this.neighbors[a].push(b)
	this.neighbors[b].push(a)
}

function reversed(values){
	var n = values.length
	var result = new Array(n)
	for (var i = 0; i < n; i++){
		result[n - 1 - i] = values[i]
	}
	return result
}

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

var nodeRadius = 25
var fontHeight = 20
context.font = fontHeight + "px Arial"

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
    context.strokeStyle = "black"
    forEach(G.nodes, function(node){
		var [ax, ay] = G.pos[node]
		forEach(G.neighbors[node], function(neighbor){
			var [bx, by] = G.pos[neighbor]
			strokeLine(ax, ay, bx, by)
		})
	})
}

function drawGraph(G){
	drawEdges(G)
	drawNodes(G)
}

//var G = makeStar(width/2, height/2, 200.0, 10)
var G = makeRandomDiskGraph(30, 150)

var logCounter = 0

function log(){
	if (logCounter >= 50) return
	logCounter++
	
	console.log.apply(console, arguments)
}

function rotate(x, y, angle){
	var c = Math.cos(angle)
	var s = Math.sin(angle)
	var x2 = c*x - s*y
	var y2 = s*x + c*y
	return [x2, y2]
}

function strokeArrow(ax, ay, bx, by){
	var dx = bx - ax
	var dy = by - ay
	
	var d = Math.sqrt(dx*dx + dy*dy)
	
	var mx = (ax + bx)*0.5
	var my = (ay + by)*0.5
	
	var r = 20
	
	var rx = r*dx/d
	var ry = r*dy/d
	
	var [vx, vy] = rotate(rx, ry, Math.PI*0.8)
	var [wx, wy] = rotate(rx, ry, Math.PI*-0.8)
	
	strokeLine(ax, ay, bx, by)
	strokeLine(mx, my, mx + vx, my + vy)
	strokeLine(mx, my, mx + wx, my + wy)
}

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

function redraw(){
    context.fillStyle = "white";
    context.fillRect(0, 0, width, height);
    
    drawEdges(G)
    context.strokeStyle = "#f00"
    
    context.lineWidth = 3
    
    if (selected != -1){
		var path = shortestPath(G, 0, selected)
		drawPath(G, path)
    }
    
    context.lineWidth = 1
    
    drawNodes(G)
    
    if (selected != -1){
        drawNode(G, selected, "#0f0")
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
    
    if (selected != -1 && mouseDown[1]){
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
        //selected = -1
    }
    
    redraw()
}

window.onmousemove = function(e){
    moveMouse(e)
    redraw()
}

redraw()
