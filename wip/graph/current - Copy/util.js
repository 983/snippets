function copy(values){
    var n = values.length
    var result = new Array(n)
    for (var i = 0; i < n; i++){
        result[i] = values[i]
    }
    return result
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

function reversed(values){
    var n = values.length
    var result = new Array(n)
    for (var i = 0; i < n; i++){
        result[n - 1 - i] = values[i]
    }
    return result
}

function contains(values, value){
    for (var i = 0; i < values.length; i++){
        if (values[i] === value) return true
    }
    return false
}

function rotate(x, y, angle){
    var c = Math.cos(angle)
    var s = Math.sin(angle)
    var x2 = c*x - s*y
    var y2 = s*x + c*y
    return [x2, y2]
}

function filter(values, shouldKeep){
    var result = []
    forEach(values, function(value){
        if (shouldKeep(value)){
            result.push(value)
        }
    })
    return result
}

function reduce(f, result, values){
    forEach(values, function(value){
        result = f(result, value)
    })
    return result
}

function min(values){
    return reduce(Math.min, Number.POSITIVE_INFINITY, values)
}

function max(values){
    return reduce(Math.max, Number.NEGATIVE_INFINITY, values)
}

function map(values, f){
    var n = values.length
    var result = new Array(n)
    for (var i = 0; i < n; i++){
        result[i] = f(values[i])
    }
    return result
}

function sign(x){
    if (x < 0) return -1
    if (x > 0) return +1
    return 0
}

function identity(value){
    return value
}

function compareNumber(a, b){
    if (a < b) return -1
    if (a > b) return 1
    return 0
}

function compareArray(a, b){
    if (a.length != b.length) throw "arrays have different lengths: " + a + ", " + b
    for (var i = a.length - 1; i >= 0; i--){
        if (a[i] < b[i]) return -1
        if (a[i] > b[i]) return +1
    }
    return 0
}

function sorted(values, key){
    var result = copy(values)
    
    if (key === undefined){
        result.sort(compareNumber)
    }else{
        result.sort((a, b) => compareNumber(key(a), key(b)))
    }
    
    return result
}
