"use strict";

var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");

function drawGrid(gridSize, nx, ny){
	context.beginPath()
	for (var ix = -nx; ix <= nx; ix++){
		var x = ix*gridSize
		context.moveTo(x, gridSize*-ny)
		context.lineTo(x, gridSize*+ny)
	}
	for (var iy = -ny; iy <= ny; iy++){
		var y = iy*gridSize
		context.moveTo(gridSize*-nx, y)
		context.lineTo(gridSize*+nx, y)
	}
	context.stroke()
}

var gridSize = 16
var nx = 64
var ny = 64
var blocked = new Uint8Array(nx*ny)
var nodeOpened = new Uint8Array(nx*ny)
var nodeClosed = new Uint8Array(nx*ny)
var f = new Float64Array(nx*ny)
var g = new Float64Array(nx*ny)
var prev = new Uint32Array(nx*ny)
var heapIndex = new Uint32Array(nx*ny)

var state = 0x12345678
function rand(){
	state ^= state << 15
	state ^= state >>> 17
	state ^= state << 5
	// to uint32
	state = state >>> 0
	return state
}

for (var i = 0; i < 1000; i++){
	var x = rand() % nx
	var y = rand() % ny
	blocked[x + y*nx] = 1
}

function isBlocked(x, y){
	return blocked[x + y*nx]
}

function findPath(startX, startY, goalX, goalY){
	for (var i = 0; i < nx*ny; i++){
		nodeOpened[i] = 0
		nodeClosed[i] = 0
		f[i] = 0
		g[i] = 0
		prev[i] = 0
		heapIndex[i] = 0
	}
	
	function isLess(a, b){
		return f[a] < f[b]
	}
	
	function setIndex(node, i){
		heapIndex[node] = i
	}
	
	var openSet = new MinHeap(isLess, setIndex)

	function distance(ax, ay, bx, by){
		var dx = ax - bx
		var dy = ay - by
		return Math.sqrt(dx*dx + dy*dy)
	}

	var goal = goalX + goalY*nx
	var start = startX + startY*nx
	nodeOpened[start] = 1
	openSet.push(start)
	
	var s = Math.sqrt(2)
	var dx = [-1,  0,  1, -1, 1, -1, 0, 1]
	var dy = [-1, -1, -1,  0, 0,  1, 1, 1]
	var dist = [s, 1, s, 1, 1, s, 1, s]
	
	var t = window.performance.now()
	
	while (!openSet.empty()){
		var node = openSet.pop()
		var nodeX = node % nx
		var nodeY = node / nx >>> 0
		nodeClosed[node] = 1
		
		if (node == goal){
			break
		}
		
		for (var k = 0; k < 8; k++){
			var x = nodeX + dx[k]
			var y = nodeY + dy[k]
			var neighbor = x + y*nx
			
			if (x < 0 || x >= nx || y < 0 || y >= ny) continue
			if (blocked[neighbor]) continue
			
			// if path is diagonal
			if (x != nodeX && y != nodeY){
				// only walk diagonal paths if they are not blocked
				if (blocked[nodeX + y*nx] || blocked[x + nodeY*nx]){
					continue
				}
			}
			
			if (nodeClosed[neighbor]) continue
			
			var gNew = g[node] + dist[k]
			
			if (nodeOpened[neighbor] && gNew >= g[neighbor]) continue
			
			prev[neighbor] = node
			g[neighbor] = gNew
			f[neighbor] = gNew + distance(x, y, nodeX, nodeY)
			
			if (nodeOpened[neighbor]){
				openSet.removeAt(heapIndex[neighbor])
			}
			nodeOpened[neighbor] = 1
			openSet.push(neighbor)
		}
	}
	
	/*
	while (!openSet.empty()){
		var node = openSet.pop()
		nodeClosed[node] = 1
		var nodeX = node % nx
		var nodeY = node / nx >>> 0
		
		// found goal
		if (node == goal){
			break
		}
		
		for (var k = 0; k < 8; k++){
			var x = nodeX + dx[k]
			var y = nodeY + dy[k]
			var neighbor = x + y*nx
			
			// do not visit nodes out of bounds
			if (x < 0 || x >= nx || y < 0 || y >= ny) continue
			if (blocked[neighbor]) continue
			
			// if path is diagonal
			if (x != nodeX && y != nodeY){
				// only walk diagonal paths if they are not blocked
				if (blocked[node.x + y*nx] || blocked[x + node.y*nx]){
					continue
				}
			}
			
			if (nodeClosed[neighbor]) continue
			
			var gNew = g[node] + Math.sqrt(dx[k]*dx[k] + dy[k]*dy[k])
			
			if (nodeOpened[neighbor] && gNew >= g[neighbor]) continue
			
			prev[neighbor] = node
			g[neighbor] = gNew
			f[neighbor] = gNew + distance(x, y, nodeX, nodeY)
			
			if (nodeOpened[neighbor]){
				openSet.removeAt(heapIndex[neighbor])
			}
			nodeOpened[neighbor] = 1
			openSet.push(neighbor)
		}
	}
	*/
	
	var dt = window.performance.now() - t
	console.log(dt)

	context.fillStyle = "#ffffff"
	context.fillRect(0, 0, canvas.width, canvas.height)
	
	for (var y = 0; y < ny; y++) for (var x = 0; x < nx; x++){
		var node = x + y*nx
		if (nodeClosed[node]){
			context.fillStyle = "#ffff00"
			context.fillRect(node.x*gridSize, node.y*gridSize, gridSize, gridSize)
		}else if (nodeOpened[node]){
			context.fillStyle = "#ffa500"
			context.fillRect(x*gridSize, y*gridSize, gridSize, gridSize)
		}else if (blocked[node]){
			context.fillStyle = "#000000"
			context.fillRect(x*gridSize, y*gridSize, gridSize, gridSize)
		}
	}

	context.fillStyle = "#00ff00"
	context.fillRect(start.x*gridSize, start.y*gridSize, gridSize, gridSize)
	context.fillRect(goal.x*gridSize, goal.y*gridSize, gridSize, gridSize)
	
	drawGrid(gridSize, nx, ny)
	
	var node = goal
	// draw path from goal to start
	context.beginPath()
	context.moveTo((goalX + 0.5)*gridSize, (goalY + 0.5)*gridSize)
	var c = 0
	while (nodeOpened[node] && node != start){
		if (c++ > nx*ny) break
		
		var x = node % nx
		var y = node / nx >>> 0
		
		context.lineTo((x + 0.5)*gridSize, (y + 0.5)*gridSize)
		node = prev[node]
	}
	
	context.strokeStyle = "#000000"
	context.lineWidth = 5
	context.stroke()
	context.lineWidth = 3
	context.strokeStyle = "#00ff00"
	context.stroke()
	context.lineWidth = 1
	context.strokeStyle = "#000000"
}

function loop(){
	findPath(0, 3, 32, 34)
	setTimeout(loop, 100)
}
loop()

/*
window.onmousemove = function(e){
    var rect = canvas.getBoundingClientRect();
    var x = e.clientX - rect.left
    var y = e.clientY - rect.top
    x = x/gridSize|0
    y = y/gridSize|0
    if (x < 0) x = 0
    if (y < 0) y = 0
    if (x >= nx) x = nx - 1
    if (y >= ny) y = ny - 1
	findPath(nx/2|0, ny/2|0, x, y)
}
*/
/*
function drawCircle(x, y, radius){
	context.beginPath();
	context.arc(x, y, radius, 0, 2*Math.PI);
	context.fill();
	context.stroke();
}

var nodes = []
var view = {scale: 1, x: 0, y: 0}
var selectedNode = null
var fontSize = 32
var padding = 16
var gridSize = fontSize + 2*padding
var connectorRadius = 8
var nodeInput = document.getElementById("node")

function selectNode(node){
	selectedNode = node
	nodeInput.value = node.text
	// TODO do this properly without delayed focus setting
	setTimeout(function(){
		nodeInput.focus()
		nodeInput.select()
	}, 50)
}

function deselectNode(){
	selectedNode = null
}

function nodeGridWidth(node){
	var width = context.measureText(node.text).width + 2*padding
	return Math.ceil(width/gridSize)
}

function nodeGridHeight(node){
	return 1
}

function nodeWidth(node){
	return nodeGridWidth(node)*gridSize
}

function nodeHeight(node){
	return nodeGridHeight(node)*gridSize
}

function snapToGrid(x){
	return Math.floor(x/gridSize)*gridSize
}

function nodeMove(node, x, y){
	node.x = snapToGrid(x)
	node.y = snapToGrid(y)
}

function createNode(x, y){
	var id = nodes.length
	
	var node = {
		x: 0,
		y: 0,
		id: id,
		text: "",
		targets: [],
	}
	
	nodeMove(node, x, y)
	nodes.push(node)
	return node
}

function forEachNode(f){
	for (var i = 0; i < nodes.length; i++){
		var node = nodes[i]
		if (!node) continue
		var result = f(node)
		if (result !== undefined) return result
	}
}

function deleteSelected(){
	if (!selectedNode) return
	
	nodes[selectedNode.id] = null
	
	forEachNode(function(node){
		node.targets.remove(selectedNode.id)
	})
	
	deselectNode()
}

function getNodeAt(x, y){
	return forEachNode(function(node){
		var x0 = node.x
		var y0 = node.y
		var x1 = node.x + nodeWidth(node)
		var y1 = node.y + nodeHeight(node)
		
		if (x0 <= x && x <= x1 && y0 <= y && y <= y1){
			return node
		}
	})
}

function drawNode(node){
	var x0 = node.x
	var y0 = node.y
	var x1 = node.x + nodeWidth(node)
	var y1 = node.y + nodeHeight(node)
	
	context.beginPath()
	context.moveTo(x0, y0)
	context.lineTo(x1, y0)
	context.lineTo(x1, y1)
	context.lineTo(x0, y1)
	context.closePath()
	context.fill()
	context.stroke()
}

function redraw(){
	// clear screen
	context.setTransform(1, 0, 0, 1, 0, 0)
	context.fillStyle = rgba(0.95, 0.95, 0.95, 1.0)
	context.fillRect(0, 0, canvas.width, canvas.height)
	context.setTransform(view.scale, 0, 0, view.scale, view.x, view.y)
	
	context.lineWidth = 1
	context.strokeStyle = "#cccccc"
	drawGrid(gridSize, 5*20)
	context.strokeStyle = "#555555"
	drawGrid(gridSize*5, 20)
	
	context.lineWidth = 3
	
	// draw node circles
	context.strokeStyle = "#000000"
	context.fillStyle = rgba(0.9, 0.9, 0.9, 1)
	
	forEachNode(function(node){
		drawNode(node)
	})
	
	// draw selected node
	if (selectedNode){
		context.fillStyle = rgba(1, 1, 0, 1)
		drawNode(selectedNode)
	}
	
	// draw node descriptions
	context.font = fontSize + "px Arial";
	context.fillStyle = "black"
	forEachNode(function(node){
		var x0 = node.x + padding
		var y0 = node.y + nodeHeight(node)*0.5 + fontSize*0.5
		context.fillText(node.text, x0, y0)
	})

if (0){
	// draw connections
	context.strokeStyle = "#000000"
	forEachNode(function(source){
		for (var i = 0; i < source.targets.length; i++){
			var target = nodes[source.targets[i]]
			var x0 = source.x + nodeWidth(source)*0.5
			var y0 = source.y
			
			var x1 = target.x - nodeWidth(target)*0.5
			var y1 = target.y
			
			context.beginPath();
			var r = 0.5*nodeHeight(source)
			
			if (source === target){
				var y = source.y - 2*r
				context.arc(x0, y0 - r, r, 0.5*Math.PI, 1.5*Math.PI, true)
				context.arc(x1, y0 - r, r, 1.5*Math.PI, 0.5*Math.PI, true)
			}else{
				var x = 0.5*(x0 + x1)
				if (source.x < target.x){
					context.moveTo(x0, y0)
					context.bezierCurveTo(x, y0, x, y1, x1, y1)
					context.stroke();
				}else{
					var y0r = y0 + 2*r
					var y1r = y1 + 2*r
					
					context.moveTo(x0, y0)
					context.arc(x0, y0 + r, r, 1.5*Math.PI, 0.5*Math.PI, false)
					
					var x2 = x0 - nodeWidth(source)
					var x3 = x1 + nodeWidth(target)
					var x = 0.5*(x2 + x3)
					
					context.lineTo(x2, y0r)
					context.bezierCurveTo(x, y0r, x, y1r, x3, y1r)
					
					context.arc(x1, y1 + r, r, 0.5*Math.PI, 1.5*Math.PI, false)
					
					context.lineTo(x1, y1)
				}
			}
			context.stroke();
		}
	})
}
	
	// draw connector dots
	context.strokeStyle = "#000000"
	context.fillStyle = "#ffffff"
	forEachNode(function(node){
		var x0 = node.x
		var x1 = node.x + nodeWidth(node)
		var y = node.y + nodeHeight(node)*0.5
		
		drawCircle(x0, y, connectorRadius)
		drawCircle(x1, y, connectorRadius)
	})
	
	requestAnimationFrame(redraw)
}

var old_x = 0
var old_y = 0

function getMouse(e){
    var rect = canvas.getBoundingClientRect();
    var x = e.clientX - rect.left
    var y = e.clientY - rect.top
    var dx = x - old_x
    var dy = y - old_y
    old_x = x
    old_y = y
    
	return {
		x: (x - view.x)/view.scale,
		y: (y - view.y)/view.scale,
		dx: dx/view.scale,
		dy: dy/view.scale,
	}
}

function translateView(dx, dy){
	view.x += dx*view.scale
	view.y += dy*view.scale
}

canvas.onmousemove = function(e){
	var mouse = getMouse(e)
	
	// drag selected node
	if (e.buttons == 1 && selectedNode){
		nodeMove(selectedNode, mouse.x, mouse.y)
	}
	
	// move view on drag
	if (e.buttons == 2){
		translateView(mouse.dx, mouse.dy)
	}
}

canvas.onmousedown = function(e){
	var p = getMouse(e)
	
	if (e.which == 1){
		var node = getNodeAt(p.x, p.y)
		
		if (!node){
			node = createNode(p.x, p.y)
		}
		
		selectNode(node)
	}
	
	if (e.which == 3){
		var node = getNodeAt(p.x, p.y)
		
		if (node){
			// if no node was selected previously
			if (selectedNode){
				// connect selected node to clicked node
				if (selectedNode.targets.contains(node.id)){
					selectedNode.targets.remove(node.id)
				}else{
					selectedNode.targets.push(node.id)
				}
			}
			// if node was selected previously
			else{
				selectNode(node)
			}
		}else{
			deselectNode(node)
		}
	}
}

canvas.onwheel = function(e){
	var p = getMouse(e)
    var wheel = e.deltaY > 0 ? 1 : -1
    var zoom = Math.pow(7.0/8.0, wheel)
    translateView(p.x, p.y)
    view.scale *= zoom
    translateView(-p.x, -p.y)
    return false
}

nodeInput.oninput = function(){
	if (!selectedNode) return
	selectedNode.text = nodeInput.value
	// nodeInput.selectionStart
}

window.oncontextmenu = function(e){
	e.preventDefault()
}

window.onkeydown = function(e){
	if (e.keyCode == 46){
		deleteSelected()
	}
}

window.onkeyup = function(e){
}
redraw()
*/
