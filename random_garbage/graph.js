"use strict";
var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");
var width = canvas.width;
var height = canvas.height;

var nextNodeIndex = 1

function Node(x, y, name){
    if (name === undefined){
        name = nextNodeIndex + ""
        nextNodeIndex++
    }
    
    this.x = x
    this.y = y
    this.name = name
}

function Edge(a, b){
    this.a = a
    this.b = b
}

function Graph(nodes, edges){
    this.nodes = nodes
    this.edges = edges
}

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

var nodeRadius = 25
var fontHeight = 20
context.font = fontHeight + "px Arial"

function drawNode(node, backgroundColor){
    context.fillStyle = backgroundColor
    fillCircle(node.x, node.y, nodeRadius)
    
    context.strokeStyle = "black"
    strokeCircle(node.x, node.y, nodeRadius)
    
    context.fillStyle = "black"
    var width = context.measureText(node.name).width
    context.fillText(node.name, node.x - 0.5*width, node.y + fontHeight*0.35)
}

function drawGraph(G){
    context.strokeStyle = "black";
    
    for (var i = 0; i < G.edges.length; i++){
        var edge = G.edges[i]
        var a = edge.a
        var b = edge.b
        strokeLine(a.x, a.y, b.x, b.y)
    }
    
    for (var i = 0; i < G.nodes.length; i++){
        var node = G.nodes[i]
        drawNode(node, "#fff")
    }
}

function make_star(xCenter, yCenter, radius, n){
    var edges = []
    var center = new Node(xCenter, yCenter)
    var nodes = [center]
    for (var i = 0; i < n; i++){
        var angle = 2*Math.PI/n*i
        
        var x = radius*Math.cos(angle) + xCenter
        var y = radius*Math.sin(angle) + yCenter
        
        var leaf = new Node(x, y)
        nodes.push(leaf)
        
        var edge = new Edge(center, leaf)
        edges.push(edge)
    }
    return new Graph(nodes, edges)
}

var G = make_star(width/2, height/2, 200.0, 10)

function redraw(){
    context.fillStyle = "white";
    context.fillRect(0, 0, width, height);
    
    drawGraph(G)
    
    if (selected){
        drawNode(selected, "#0f0")
    }
}

redraw();

var mouseOld = {x:0, y:0}
var mouse = {x:0, y:0}
var mouseDown = {}

var selected = null

function moveMouse(e){
    var rect = canvas.getBoundingClientRect()
    var x = e.clientX - rect.left
    var y = e.clientY - rect.top
    
    var dx = x - mouse.x
    var dy = y - mouse.y
    
    mouseOld = mouse
    mouse = {x:x, y:y}
    
    if (selected !== null && mouseDown[1]){
        selected.x += dx
        selected.y += dy
    }
}

function distance(a, b){
    var dx = a.x - b.x
    var dy = a.y - b.y
    return Math.sqrt(dx*dx + dy*dy)
}

window.onmousedown = function(e){
    moveMouse(e)
    
    mouseDown[e.which] = true
    
    if (e.which == 1){
        selected = null
        for (var i = 0; i < G.nodes.length; i++){
            var node = G.nodes[i]
            if (distance(node, mouse) < nodeRadius){
                selected = node
            }
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
        //selected = null
    }
    
    redraw()
}

window.onmousemove = function(e){
    moveMouse(e)
    redraw()
}
