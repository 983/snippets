var canvas = document.getElementById("myCanvas")
var context = canvas.getContext("2d")

function drawCircle(x, y, radius){
    context.beginPath()
    context.arc(x, y, radius, 0, 2*Math.PI)
    context.stroke()
}

function drawLine(ax, ay, bx, by){
    context.beginPath()
    context.moveTo(ax, ay)
    context.lineTo(bx, by)
    context.stroke()
}

function last(values){
    return values[values.length - 1]
}

function copy(values){
    var n = values.length
    var result = new Array(n)
    for (var i = 0; i < n; i++){
        result[i] = values[i]
    }
    return result
}

function polyPoint(poly, dist){
    var sum = 0
    
    var a = last(poly)
    for (var i = 0; i < poly.length; i++){
        var b = poly[i]
        
        result += distance(a, b)
        
        a = b
    }
}

function pointOnLine(a, b, dist){
    var [ax, ay] = a
    var [bx, by] = b
    
    var dx = bx - ax
    var dy = by - ay
    
    var d = Math.sqrt(dx*dx + dy*dy)
    
    var nx = 1.0/d * dx
    var ny = 1.0/d * dy
    
    var x = ax + dist*nx
    var y = ay + dist*ny
    
    return [x, y]
}

function equidistantPoints(poly, n){
    var distanceSum = new Float64Array(poly.length + 1)
    
    var a = last(poly)
    for (var i = 0; i < poly.length; i++){
        var b = poly[i]
        
        distanceSum[i + 1] = distanceSum[i] + distance(a, b)
        
        a = b
    }
    
    var perimeter = last(distanceSum)
    
    var points = []
    
    var distanceNextPoint = 0.0
    
    a = last(poly)
    
    // TODO ensure we don't skip the last point due to floating point inaccuracy
    
    for (var i = 0; i < poly.length; i++){
        var b = poly[i]
        
        while (points.length < n && distanceSum[i + 1] >= distanceNextPoint){
        
            var p = pointOnLine(a, b, distanceNextPoint - distanceSum[i])
            
            points.push(p)
            
            distanceNextPoint += perimeter/n
        }
        
        a = b
    }
    
    return points
}

var precision = document.getElementById("precision")
precision.value = 5

precision.onchange = function(e){
    circles = new Circles(poly)
}

function Circles(poly){
    var n = 1024
    
    var points = equidistantPoints(poly, n)

    var real = new Float64Array(n)
    var imag = new Float64Array(n)

    for (var i = 0; i < n; i++){
        var [x, y] = points[i]
        real[i] = x
        imag[i] = y
    }
    
    var fft = new FFT(n)
    fft.fft(real, imag)
    
    fft.scale(real, imag)
    
    var amplitudes = new Float64Array(n)
    
    for (var i = 0; i < n; i++){
        amplitudes[i] = Math.sqrt(real[i]*real[i] + imag[i]*imag[i])
    }
    
    var threshold = precision.value|0
    
    var indices = argsort(amplitudes)
    
    indices = reversed(indices)
    
    for (var j = threshold; j < n; j++){
        var i = indices[j]
        real[i] = 0
        imag[i] = 0
    }
    
    var x = copy(real)
    var y = copy(imag)
    
    fft.ifft(x, y)
    
    this.real = real
    this.imag = imag
    this.n = n
    this.indices = indices
    this.threshold = threshold
    this.x = x
    this.y = y
}

var poly = [[-16,-147],[-17,-144],[-21,-139],[-22,-136],[-23,-133],[-28,-126],[-29,-123],[-30,-120],[-32,-117],[-34,-110],[-35,-107],[-36,-104],[-37,-101],[-40,-94],[-41,-91],[-42,-88],[-44,-85],[-46,-78],[-47,-75],[-48,-72],[-49,-69],[-50,-66],[-51,-61],[-52,-58],[-53,-55],[-54,-52],[-56,-43],[-57,-40],[-58,-37],[-59,-34],[-62,-25],[-64,-20],[-65,-17],[-66,-14],[-68,-8],[-69,-5],[-71,-2],[-72,2],[-74,8],[-75,11],[-76,14],[-77,19],[-78,22],[-79,25],[-81,32],[-82,35],[-83,38],[-84,41],[-85,47],[-86,50],[-86,54],[-87,58],[-88,61],[-89,64],[-90,69],[-91,72],[-91,76],[-92,80],[-92,84],[-93,91],[-93,95],[-95,101],[-95,105],[-95,109],[-95,113],[-95,119],[-96,122],[-93,124],[-84,125],[-80,125],[-77,126],[-73,126],[-64,126],[-60,126],[-57,125],[-53,125],[-44,122],[-40,122],[-37,120],[-34,119],[-30,118],[-21,116],[-18,115],[-15,113],[-12,112],[-4,108],[-1,107],[2,105],[5,104],[13,99],[16,97],[19,95],[22,93],[25,91],[28,89],[30,86],[37,81],[40,79],[43,77],[45,74],[50,70],[52,67],[55,65],[60,60],[62,57],[65,55],[68,53],[70,50],[73,49],[77,43],[80,41],[82,38],[85,35],[88,32],[90,29],[94,25],[97,22],[100,20],[103,15],[106,12],[108,9],[111,7],[115,2],[117,-1],[119,-4],[124,-9],[126,-12],[129,-14],[135,-22],[137,-25],[140,-28],[144,-32],[146,-35],[148,-38],[150,-41],[152,-44],[154,-47],[157,-52],[160,-54],[161,-57],[164,-59],[166,-62],[163,-63],[159,-63],[147,-64],[143,-64],[139,-64],[135,-64],[124,-64],[119,-64],[115,-64],[110,-64],[98,-64],[94,-64],[90,-64],[86,-64],[78,-64],[74,-64],[70,-64],[67,-63],[58,-63],[54,-63],[50,-63],[41,-63],[37,-63],[33,-63],[29,-63],[20,-63],[16,-63],[12,-63],[9,-64],[0,-64],[-4,-64],[-8,-64],[-12,-64],[-18,-64],[-22,-64],[-25,-65],[-35,-65],[-38,-66],[-42,-66],[-53,-66],[-57,-66],[-61,-66],[-68,-66],[-72,-66],[-76,-66],[-83,-66],[-87,-66],[-91,-66],[-96,-66],[-99,-65],[-103,-65],[-108,-65],[-112,-65],[-118,-65],[-122,-65],[-125,-64],[-124,-61],[-122,-56],[-119,-54],[-118,-50],[-116,-46],[-113,-44],[-110,-40],[-107,-37],[-106,-34],[-101,-30],[-99,-27],[-96,-25],[-92,-19],[-89,-17],[-87,-14],[-82,-10],[-79,-7],[-77,-4],[-72,-1],[-70,2],[-67,4],[-61,9],[-58,11],[-56,14],[-53,15],[-47,19],[-44,21],[-41,25],[-38,27],[-35,29],[-32,32],[-29,35],[-26,36],[-19,42],[-16,44],[-14,47],[-11,48],[-8,50],[-4,53],[-1,54],[2,56],[5,58],[10,61],[13,62],[16,64],[24,69],[27,70],[30,72],[37,76],[40,77],[43,80],[46,81],[54,86],[56,89],[59,90],[62,92],[70,96],[73,98],[76,100],[83,104],[86,105],[89,106],[96,109],[99,111],[102,113],[109,116],[112,117],[115,119],[121,121],[124,122],[128,125],[131,126],[136,128],[139,129],[143,130]]
var circles = new Circles(poly)

var logCounter = 0

function log(){
    if (logCounter++ < 10){
        console.log.apply(console, arguments)
    }
}

function clamp(x, a, b){
    if (x < a) return a
    if (x > b) return b
    return x
}

function compare(a, b){
    if (a < b) return -1
    if (a > b) return +1
    return 0
}

function argsort(values){
    var n = values.length
    var indices = new Int32Array(n)
    for (var i = 0; i < n; i++) indices[i] = i
    
    // compare by index
    indices.sort(function(i, j){
        return compare(values[i], values[j])
    })
    
    return indices
}

function reversed(values){
    var n = values.length
    var result = new Array(n)
    for (var i = 0; i < n; i++){
        result[i] = values[n - 1 - i]
    }
    return result
}

Circles.prototype.draw = function(){
    var n = this.n
    var real = this.real
    var imag = this.imag
    
    var x = 0
    var y = 0
    
    var t = window.performance.now()*0.0001
    
    t = t - Math.floor(t)
    
    var offset = clamp(n*t|0, 0, n - 1)
    
    context.globalAlpha = 0.3
    
    var indices = this.indices
    
    for (var j = 0; j < this.threshold; j++){
        var i = indices[j]
        
        var angle = 2*Math.PI*i/n*offset
        
        var u = real[i]
        var v = imag[i]
        
        var c = Math.cos(angle)
        var s = Math.sin(angle)
        
        var dx = u*c - v*s
        var dy = u*s + v*c
        
        var radius = Math.sqrt(dx*dx + dy*dy)
        
        context.strokeStyle = "#000"
        drawCircle(x, y, radius)
        context.strokeStyle = "#0f0"
        drawLine(x, y, x + dx, y + dy)
        
        x += dx
        y += dy
    }
    
    context.globalAlpha = 1.0
    
    context.strokeStyle = "#000"
    context.beginPath()
    for (var i = 0; i < n; i++){
        var x = this.x[i]
        var y = this.y[i]
        context.lineTo(x, y)
    }
    context.closePath()
    context.stroke()
}

function redraw(){

    context.setTransform(1, 0, 0, 1, 0, 0)
    context.translate(canvas.width*0.5, canvas.height*0.5)
    
    context.fillStyle = "#abc"
    context.fillRect(-1024, -1024, 2048, 2048)
    
    if (circles){
        circles.draw()
        var points = equidistantPoints(poly, 10)
        /*
        context.strokeStyle = "#ff0"
        for (var i = 0; i < points.length; i++){
            var [x, y] = points[i]
            drawCircle(x, y, 5)
        }
        */
    }else{
        context.strokeStyle = "#000"
        context.beginPath()
        if (poly.length > 0){
            for (var i = 0; i < poly.length; i++){
                var [x, y] = poly[i]
                context.lineTo(x, y)
            }
            context.closePath()
        }
        context.stroke()
    }
    
    setTimeout(redraw, 20)
}

redraw()

function distance(a, b){
    var [ax, ay] = a
    var [bx, by] = b
    var dx = ax - bx
    var dy = ay - by
    return Math.sqrt(dx*dx + dy*dy)
}

canvas.onmousemove = function(e){
    var rect = canvas.getBoundingClientRect()
    var x = e.clientX - rect.left
    var y = e.clientY - rect.top
    
    if (e.buttons & 1){
        var width = canvas.width
        var height = canvas.height
        
        x -= width*0.5
        y -= height*0.5
        
        if (poly.length == 0 || distance([x, y], last(poly)) > 3)
        {
            poly.push([x, y])
        }
    }
}

canvas.onmousedown = function(e){
    if (e.which == 1){
        poly = []
        circles = null
    }
}

canvas.onmouseup = function(e){
    if (e.which == 1){
        circles = new Circles(poly)
    }
}
