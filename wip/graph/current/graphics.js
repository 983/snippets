var canvas = document.getElementById("canvas")
var context = canvas.getContext("2d")
var width = canvas.width
var height = canvas.height

var fontHeight = 20
context.font = fontHeight + "px Arial"

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
