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

function Node(x, y){
	this.x = x
	this.y = y
	this.prev = null
	this.f = 0
	this.g = 0
	this.opened = false
	this.closed = false
	this.neighbors = []
	this.heapIndex = -1
}

function graphFromGrid(blocked, nx, ny){
	var nodes = new Array(nx*ny)

	for (var y = 0; y < ny; y++){
		for (var x = 0; x < nx; x++){
			nodes[x + y*nx] =  new Node(x, y)
		}
	}

	var dx = [-1, 0, 1, -1, 1, -1, 0, 1]
	var dy = [-1, -1, -1, 0, 0, 1, 1, 1]

	for (var y = 0; y < ny; y++){
		for (var x = 0; x < nx; x++){
			if (blocked[x + y*nx]) continue
			
			var node = nodes[x + y*nx]
			
			for (var i = 0; i < 8; i++){
				
				var x2 = x + dx[i]
				var y2 = y + dy[i]
				
				if (x2 < 0 || y2 < 0 || x2 >= nx || y2 >= ny) continue
				
				if (blocked[x2 + y2*nx]) continue
				
				if (x != x2 && y != y2){
					if (blocked[x2 + y*nx] || blocked[x + y2*nx]){
						continue
					}
				}
				
				var neighbor = nodes[x2 + y2*nx]
				
				node.neighbors.push(neighbor)
			}
		}
	}
	
	return nodes
}

function findPath(nodes, start, end){
	if (!start || !end) return false
	
	for (var i = 0; i < nodes.length; i++){
		var node = nodes[i]
		if (node){
			node.opened = false
			node.closed = false
			node.f = 0
			node.g = 0
			node.prev = null
			node.heapIndex = -1
		}
	}
	
	function isLess(a, b){
		return a.f < b.f
	}
	
	function setIndex(node, i){
		node.heapIndex = i
	}
	
	function distance(a, b){
		var dx = a.x - b.x
		var dy = a.y - b.y
		return Math.sqrt(dx*dx + dy*dy)
	}
	
	var openSet = new MinHeap(isLess, setIndex)
	
	start.opened = 1
	openSet.push(start)
	
	while (!openSet.empty()){
		var node = openSet.pop()
		node.closed = true
		
		if (node == end) return true
		
		for (var k = 0; k < node.neighbors.length; k++){
			var neighbor = node.neighbors[k]
			
			if (neighbor.closed) continue
			
			var gNew = node.g + distance(node, neighbor)
			
			if (neighbor.opened && gNew >= neighbor.g) continue
			
			neighbor.prev = node
			neighbor.g = gNew
			neighbor.f = gNew + distance(neighbor, end)
			
			if (neighbor.opened){
				openSet.removeAt(neighbor.heapIndex)
			}
			neighbor.opened = true
			openSet.push(neighbor)
		}
	}
	
	return false
}

var nx = 64
var ny = 64
var blocked = new Uint8Array(nx*ny)

var mouseOld = {x: 0, y: 0}
var mouse = {x: 0, y: 0}

function updateMouse(e){
    var rect = canvas.getBoundingClientRect();
	mouseOld = mouse
	mouse = {
		x: e.clientX - rect.left,
		y: e.clientY - rect.top,
	}
}

window.onmousedown = function(e){
	updateMouse(e)
}

function drawLine(x0, y0, x1, y1, onPixel){
	var dx = Math.abs(x1 - x0)
	var dy = -Math.abs(y1 - y0)
	var sx = x0 < x1 ? 1 : -1
	var sy = y0 < y1 ? 1 : -1
	var err = dx + dy;
	while (1){
		onPixel(x0, y0)
		if (x0 == x1 && y0 == y1) break
		var e2 = err*2
		if (e2 > dy){
			err += dy
			x0 += sx
		}
		if (e2 < dx){
			err += dx
			y0 += sy
		}
	}
}

window.onmousemove = function(e){
	updateMouse(e)
	
	var gridSize = 16

	function screenToGrid(p){
		var x = p.x/gridSize|0
		var y = p.y/gridSize|0
		if (x < 0) x = 0
		if (y < 0) y = 0
		if (x >= nx) x = nx - 1
		if (y >= ny) y = ny - 1
		return {x:x, y:y}
	}
	
    var endPoint = screenToGrid(mouse)
    
    if (e.buttons != 0){
		var oldPoint = screenToGrid(mouseOld)
		drawLine(oldPoint.x, oldPoint.y, endPoint.x, endPoint.y, function(x, y){
			blocked[x + y*nx] = true
		})
		blocked[endPoint.x + endPoint.y*nx] = false
	}
	
	var nodes = graphFromGrid(blocked, nx, ny)
	
	var startPoint = {x: nx/2|0, y: ny/2|0}
	var start = nodes[startPoint.x + startPoint.y*nx]
	var end = nodes[endPoint.x + endPoint.y*nx]
	
	if (findPath(nodes, start, end)){
		context.clearRect(0, 0, canvas.width, canvas.height);
	
		context.fillStyle = "#ffff00"
		for (var y = 0; y < ny; y++) for (var x = 0; x < nx; x++){
			var node = nodes[x + y*nx]
			if (node && node.opened){
				context.fillRect(x*gridSize, y*gridSize, gridSize, gridSize)
			}
		}
		context.fillStyle = "#ffa500"
		for (var y = 0; y < ny; y++) for (var x = 0; x < nx; x++){
			var node = nodes[x + y*nx]
			if (node && node.closed){
				context.fillRect(x*gridSize, y*gridSize, gridSize, gridSize)
			}
		}
		context.fillStyle = "#000000"
		for (var y = 0; y < ny; y++) for (var x = 0; x < nx; x++){
			if (blocked[x + y*nx]){
				context.fillRect(x*gridSize, y*gridSize, gridSize, gridSize)
			}
		}

		context.fillStyle = "#00ff00"
		context.fillRect(start.x*gridSize, start.y*gridSize, gridSize, gridSize)
		context.fillRect(end.x*gridSize, end.y*gridSize, gridSize, gridSize)
		
		drawGrid(gridSize, nx, ny)
		
		var node = end
		// draw path from goal to start
		context.beginPath()
		context.moveTo((node.x + 0.5)*gridSize, (node.y + 0.5)*gridSize)
		while (node && node.opened){
			
			context.lineTo((node.x + 0.5)*gridSize, (node.y + 0.5)*gridSize)
			
			if (node == start) break
			
			node = node.prev
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
}

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
