var canvas = document.getElementById("canvas")
var context = canvas.getContext("2d")

var images = {}
var nImages = 0
/*
function sum2(values){
    var result = 0
    for (var i = 0; i < values.length; i++){
        var value = values[i]
        result += value*value
    }
    return result
}

function matshow(A){
    var width = A.n
    var height = A.m
    
    if (width > 1000 || height > 1000){
        console.log(A)
        throw ["matrix too large to show:", A.m, A.n]
    }
    
    canvas.width = width
    canvas.height = height
    
    var imageData = context.createImageData(width, height)
    
    var a = Math.min.apply(null, A.values)
    var b = Math.max.apply(null, A.values)
    
    console.log("min:", a, "max:", b, "shape:", height, width)
    
    for (var i = 0; i < width*height; i++){
        var u = (A.values[i] - a)/(b - a)
        var gray = u*255
        imageData.data[i*4 + 0] = gray
        imageData.data[i*4 + 1] = gray
        imageData.data[i*4 + 2] = gray
        imageData.data[i*4 + 3] = 0xff
    }
    
    context.putImageData(imageData, 0, 0)
    
    throw "matshow"
}

function testDebug(a){
    var D = new DenseMatrix(debug.length, 1, debug)
    D = sub(D, a)
    console.log("debug error:", D.sum2())
    throw "stop"
}
*/
function solveChannel(image, alpha, width, height){
    var alphaf = alpha.flatten()
    
    var d = ones(height, width)
    // clear d[:, 0]
    for (var i = 0; i < height; i++){
        d.values[i*width] = 0
    }
    // clear d[0, :]
    for (var j = 0; j < width; j++){
        d.values[j] = 0
    }
    d = d.flatten()
    
    var I  = new DiagonalMatrix(d)
    var Dx = new DiagonalMatrix(d.slice(1).neg(), -1)
    var Dy = new DiagonalMatrix(d.slice(width).neg(), -width)
    
    var w0 = add(dot(I, alphaf), dot(Dx, alphaf)).abs().sqrt()
    var w1 = add(dot(I, alphaf), dot(Dy, alphaf)).abs().sqrt()
    
    w0 = new DiagonalMatrix(w0)
    w1 = new DiagonalMatrix(w1)
    
    var n = width*height
    
    var A0 = new DiagonalMatrix(alphaf)
    var A1 = new DiagonalMatrix(sub(1, alphaf))
    var A2 = new SumMatrix([dot(w0, I), dot(w0, Dx)], n, n)
    var A3 = new SumMatrix([dot(w1, I), dot(w1, Dy)], n, n)
    
    var matrices = [
        new BlockMatrix(A0, 0*n, 0*n, 5*n, 2*n), new BlockMatrix(A1, 0*n, 1*n, 5*n, 2*n),
        new BlockMatrix(A2, 1*n, 0*n, 5*n, 2*n),
        new BlockMatrix(A3, 2*n, 0*n, 5*n, 2*n),
                                                 new BlockMatrix(A2, 3*n, 1*n, 5*n, 2*n),
                                                 new BlockMatrix(A3, 4*n, 1*n, 5*n, 2*n),
    ]
    
    var A = new SumMatrix(matrices, 5*n, 2*n)
    
    var b = new DenseMatrix(5*n, 1)
    for (var i = 0; i < n; i++){
        b.values[i] = image.values[i]
    }
    
    var max_iterations = 100
    
    var x = leastSquares(A, b, max_iterations)
    
    var foreground = x.slice(0, n).reshape(height, width)
    
    return foreground
}

function refineAlpha(alpha, trimapImageData, imageImageData){
    //var alphaImageData  = images[ "alpha.png"]
    //var trimapImageData = images["trimap.png"]
    //var imageImageData  = images[ "image.png"]
    
    // TODO red, green, blue to image
    
    var width  = imageImageData.width
    var height = imageImageData.height
    
    //var alpha  = new DenseMatrix(height, width)
    var trimap = new DenseMatrix(height, width)
    var red    = new DenseMatrix(height, width)
    var green  = new DenseMatrix(height, width)
    var blue   = new DenseMatrix(height, width)
    
    for (var i = 0; i < width*height; i++){
        //alpha .values[i] = alphaImageData .data[i*4 + 0]/255.0
        trimap.values[i] = trimapImageData.data[i*4 + 0]/255.0
        red   .values[i] = imageImageData .data[i*4 + 0]/255.0
        green .values[i] = imageImageData .data[i*4 + 1]/255.0
        blue  .values[i] = imageImageData .data[i*4 + 2]/255.0
    }
    
    var red_foreground   = solveChannel(red  , alpha, width, height)
    var green_foreground = solveChannel(green, alpha, width, height)
    var blue_foreground  = solveChannel(blue , alpha, width, height)
    
    var imageData = context.createImageData(width, height)
    
    for (var i = 0; i < width*height; i++){
        
        var alf = alpha.values[i]
        var red   = alf*red_foreground.values[i]
        var green = alf*green_foreground.values[i]
        var blue  = alf*blue_foreground.values[i]
        
        red   = clamp(red  *255, 0, 255)
        green = clamp(green*255, 0, 255)
        blue  = clamp(blue *255, 0, 255)
        
        imageData.data[i*4 + 0] = red
        imageData.data[i*4 + 1] = green
        imageData.data[i*4 + 2] = blue
        imageData.data[i*4 + 3] = 0xff
    }
    context.putImageData(imageData, width, 0)
}

function findBoundary(A){
    var positions = []
    for (var i = 1; i < A.m - 1; i++){
        for (var j = 1; j < A.n - 1; j++){
            if (A.get(i, j)){
                if (
                    !A.get(i - 1, j    ) ||
                    !A.get(i + 1, j    ) ||
                    !A.get(i    , j - 1) ||
                    !A.get(i    , j + 1)
                ){
                    positions.push([i, j])
                }
            }
        }
    }
    return positions
}

function calculateAlpha(F, B, I){
    var numerator   = 0.0
    var denominator = 1e-6
    
    for (var c = 0; c < 3; c++){
        var f = F[c]
        var b = B[c]
        var i = I[c]
        
        numerator   += (i - b)*(f - b)
        denominator += (f - b)*(f - b)
    }
    
    return clamp(numerator/denominator, 0, 1)
}

function colorCost(F, B, I, alpha){
    var result = 0.0
    for (var c = 0; c < 3; c++){
        var f = F[c]
        var b = B[c]
        var i = I[c]
        
        result += sqr(i - (alpha*f + (1.0 - alpha)*b))
    }
    
    return Math.sqrt(result)
}

function distCost(i0, j0, i1, j1){
    return Math.sqrt(sqr(i0 - i1) + sqr(j0 - j1))
}

function Sample(fi, bj, df, db){
    this.fi = fi
    this.bj = bj
    this.df = df
    this.db = db
    this.cost = Number.POSITIVE_INFINITY
    this.alpha = -1.0
}

function calculateAlphaFromImageAndTrimap(trimapImageData, imageImageData){
    var width  = imageImageData.width
    var height = imageImageData.height
    
    var alpha      = new DenseMatrix(height, width)
    var trimap     = new DenseMatrix(height, width)
    
    var known      = new DenseMatrix(height, width)
    var unknown    = new DenseMatrix(height, width)
    var inside     = new DenseMatrix(height, width)
    var outside    = new DenseMatrix(height, width)
    
    var image = new DenseMatrix(height, width, new Array(width*height))
    
    for (var i = 0; i < width*height; i++){
        trimap.values[i] = trimapImageData.data[i*4 + 0]/255.0
        
        var r = imageImageData.data[i*4 + 0]/255.0
        var g = imageImageData.data[i*4 + 1]/255.0
        var b = imageImageData.data[i*4 + 2]/255.0
        
        image.values[i] = [r, g, b]
        
        var value = trimap.values[i]
        
        inside.values[i]  = value < 0.1
        outside.values[i] = value > 0.9
        
        known.values[i] = inside.values[i] || outside.values[i]
        unknown.values[i] = !known.values[i]
    }
    
    var foregroundBoundary = findBoundary(inside)
    var backgroundBoundary = findBoundary(outside)
    
    var n = foregroundBoundary.length + backgroundBoundary.length
    for (var k = 0; k < n; k++){
        var i = randi(height)
        var j = randi(width)
        
        if (outside.get(i, j)){
            backgroundBoundary.push([i, j])
        }
        if (inside.get(i, j)){
            foregroundBoundary.push([i, j])
        }
    }
    
    function getIntensity(ij){
        var [i, j] = ij
        var [r, g, b] = image.get(i, j)
        return r + g + b
    }

    function compareIntensity(a, b){
        return compare(getIntensity(a), getIntensity(b))
    }
    
    foregroundBoundary.sort(compareIntensity)
    backgroundBoundary.sort(compareIntensity)
    
    function approximateNearestDistance(boundary, i, j){
        /*
        // correct nearest distance
        var minDistSquared = Number.POSITIVE_INFINITY
        for (var k = 0; k < boundary.length; k++){
            var [i2, j2] = boundary[k]
            var distSquared = sqr(i - i2) + sqr(j - j2)
            minDistSquared = Math.min(minDistSquared, distSquared)
        }
        return Math.sqrt(minDistSquared)
        */
        // Is a random point the closest point?
        // Probably not, but it works anyway :D
        var [i2, j2] = randomChoice(boundary)
        return distCost(i, j, i2, j2)
    }
    
    var coords = new DenseMatrix(height, width, new Array(width*height))
    
    var samples = new DenseMatrix(height, width, new Array(width*height))
    
    var t = window.performance.now()
    
    for (var i = 0; i < height; i++){
        for (var j = 0; j < width; j++){
            coords.set(i, j, [i, j])
            
            if (unknown.get(i, j)){
                samples.set(i, j, new Sample(
                    randi(foregroundBoundary.length),
                    randi(backgroundBoundary.length),
                    approximateNearestDistance(foregroundBoundary, i, j),
                    approximateNearestDistance(backgroundBoundary, i, j)))
            }
        }
    }
    
    console.log(window.performance.now() - t, "milliseconds")
    
    for (var iteration = 0; iteration < 4; iteration++){
        var t = window.performance.now()
        
        randomShuffle(coords.values)
        
        for (var k = 0; k < width*height; k++){
            var [i, j] = coords.values[k]
            
            if (known.get(i, j)) continue
            
            var s = samples.get(i, j)
            var I = image.get(i, j)

            for (var i2 = i - 1; i2 <= i + 1; i2++){
                for (var j2 = j - 1; j2 <= j + 1; j2++){
                    if (i2 == i && j2 == j) continue
                    if (!known.withinBounds(i2, j2)) continue
                    if (known.get(i2, j2)) continue
                    
                    var s2 = samples.get(i2, j2)
                    
                    var [fpi, fpj] = foregroundBoundary[s2.fi]
                    var [bpi, bpj] = backgroundBoundary[s2.bj]
                    
                    var F = image.get(fpi, fpj)
                    var B = image.get(bpi, bpj)
                    
                    var alphaValue = calculateAlpha(F, B, I)
                    
                    var d0 = distCost(i, j, fpi, fpj)/s.df
                    var d1 = distCost(i, j, bpi, bpj)/s.db
                    var cost = colorCost(F, B, I, alphaValue) + d0 + d1
                    
                    if (cost < s.cost){
                        s.fi = s2.fi
                        s.bj = s2.bj
                        s.cost = cost
                        s.alpha = alphaValue
                    }
                }
            }
        }
        
        var w2 = Math.max(foregroundBoundary.length, backgroundBoundary.length)
        
        for (var i = 0; i < height; i++){
            for (var j = 0; j < width; j++){
                if (known.get(i, j)) continue
                
                var s = samples.get(i, j)
                var I = image.get(i, j)
                
                for (var r = w2; r >= 1; r *= 0.5){
                    var di = randi(r)|0
                    var dj = randi(r)|0
                    
                    var fi = s.fi + di
                    var bj = s.bj + dj
                    
                    if (fi < 0 || fi >= foregroundBoundary.length || bj < 0 || bj >= backgroundBoundary.length) continue
                    
                    var [fpi, fpj] = foregroundBoundary[fi]
                    var [bpi, bpj] = backgroundBoundary[bj]
                    
                    var F = image.get(fpi, fpj)
                    var B = image.get(bpi, bpj)
                    
                    var alphaValue = calculateAlpha(F, B, I)
                    
                    var d0 = distCost(i, j, fpi, fpj)/s.df
                    var d1 = distCost(i, j, bpi, bpj)/s.db
                    var cost = colorCost(F, B, I, alphaValue) + d0 + d1
                    
                    if (cost < s.cost){
                        s.fi = fi
                        s.bj = bj
                        s.cost = cost
                        s.alpha = alphaValue
                    }
                }
            }
        }
        
        var dt = window.performance.now() - t
        
        console.log("iteration", iteration, dt, "milliseconds")
    }
    
    
    canvas.width = width*2
    canvas.height = height
    
    var imageData = context.createImageData(width, height)
    
    for (var i = 0; i < width*height; i++){
        var sample = samples.values[i]
        
        var alphaValue = trimap.values[i]
        
        if (sample){
            alphaValue = 1 - sample.alpha
        }
        
        alpha.values[i] = alphaValue
        
        var gray = clamp(alphaValue*255, 0, 255)
        
        var [r, g, b] = image.values[i]
        //r = g = b = 1
        
        imageData.data[i*4 + 0] = r*gray
        imageData.data[i*4 + 1] = g*gray
        imageData.data[i*4 + 2] = b*gray
        imageData.data[i*4 + 3] = 0xff
    }
    context.putImageData(imageData, 0, 0)
    
    //throw "stop"
    
    setTimeout(function(){
        refineAlpha(alpha, trimapImageData, imageImageData)
    }, 0)
}


var imagePath = "dandelion/image.png"
var trimapPath = "dandelion/trimap.png"

function loadImage(src){
    var image = new Image()
    image.src = src
    image.onload = function(){
        canvas.width = image.width
        canvas.height = image.height
        context.drawImage(image, 0, 0)
        //var imageData = context.createImageData(image.width, image.height)
        var imageData = context.getImageData(0, 0, image.width, image.height)
        
        var width = 512
        var height = 512
        canvas.width = width
        canvas.height = height
        context.fillStyle = "#0f0"
        context.fillRect(0, 0, width, height)
        
        images[src] = imageData
        nImages++
        
        if (nImages == 2){
            var imageImageData  = images[imagePath]
            var trimapImageData = images[trimapPath]
    
            calculateAlphaFromImageAndTrimap(trimapImageData, imageImageData)
        }
    }
}

loadImage(imagePath)
loadImage(trimapPath)

/*
var width = canvas.width
var height = canvas.height
var imageData = context.createImageData(width, height)
var rgba = imageData.data
for (var i = 0; i < width*height*4; i += 4){
    var gray = Math.random()*256
    rgba[i + 0] = gray
    rgba[i + 1] = gray
    rgba[i + 2] = gray
    rgba[i + 3] = 0xff
}

context.putImageData(imageData, 0, 0)

*/
