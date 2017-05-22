function assertSmall(value, eps){
    if (eps === undefined){
        eps = 1e-10
    }
    
    assertNumber(value)
    
    if (value > eps){
        console.log("ERROR: error too large:", value)
        errorTooLarge
    }
}

function assertNumber(x){
    if (x.constructor !== Number){
        console.log("ERROR:", x, "is not a number")
        failWithStackTrace
    }
    if (Number.isNaN(x)){
        console.log("ERROR:", x, "is NaN")
        failWithStackTrace
    }
}

function randi(n){
    return Math.floor(Math.random()*n)
}

function clamp(x, a, b){
    return Math.max(a, Math.min(b, x))
}

function sqr(x){
    return x*x
}

function compare(a, b){
    if (a < b) return -1
    if (a > b) return +1
    return 0
}

function randomChoice(values){
    return values[randi(values.length)]
}

function swap(values, i, j){
    var temp = values[i]
    values[i] = values[j]
    values[j] = temp
}

function randomShuffle(values){
    for (var i = values.length - 1; i >= 1; i--){
        var j = randi(i)
        swap(values, i, j)
    }
}
