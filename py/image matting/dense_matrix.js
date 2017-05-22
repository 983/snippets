function DenseMatrix(m, n, values){
    assertNumber(m)
    assertNumber(n)
    
    if (values === undefined){
        values = new Float64Array(m*n)
    }
    
    this.m = m
    this.n = n
    this.values = values
}

DenseMatrix.prototype.get = function(i, j){
    return this.values[i*this.n + j]
}

DenseMatrix.prototype.set = function(i, j, value){
    this.values[i*this.n + j] = value
}

DenseMatrix.prototype.transpose = function(){
    var A = this
    var B = new DenseMatrix(A.n, A.m)
    for (var i = 0; i < A.m; i++){
        for (var j = 0; j < A.n; j++){
            B.set(j, i, A.get(i, j))
        }
    }
    return B
}

DenseMatrix.prototype.slice = function(i0, i1){
    
    if (this.n !== 1){
        throw "slice only makes sense on vectors, not matrices"
    }
    
    if (i1 === undefined){
        i1 = this.m
    }
    
    return new DenseMatrix(i1 - i0, 1, this.values.slice(i0, i1))
}

DenseMatrix.prototype.apply = function(f){
    var C = this.copy()
    for (var i = 0; i < C.m*C.n; i++){
        C.values[i] = f(C.values[i])
    }
    return C
}

DenseMatrix.prototype.withinBounds = function(i, j){
    return i >= 0 && i < this.m && j >= 0 && j < this.n
}

DenseMatrix.prototype.reshape = function(m, n){
    if (m*n != this.m*this.n){
        throw ["reshaped matrix must have same number of elements", this.m, this.n, "to", m, n]
    }
    return new DenseMatrix(m, n, this.values)
}

DenseMatrix.prototype.neg = function(){
    return this.apply(function(value){ return -value })
}

DenseMatrix.prototype.abs = function(){
    return this.apply(Math.abs)
}

DenseMatrix.prototype.sqrt = function(){
    return this.apply(Math.sqrt)
}

function dotDenseDense(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, B.n)
    }
    
    var k = 0
    for (var i = 0; i < C.m; i++){
        for (var j = 0; j < C.n; j++){
            var sum = 0
            for (var k = 0; k < A.n; k++){
                sum += A.get(i, k) * B.get(k, j)
            }
            C.set(i, j, sum)
        }
    }
    
    return C
}

function addDenseDense(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, A.n)
    }
    for (var i = 0; i < A.m*A.n; i++){
        C.values[i] = A.values[i] + B.values[i]
    }
    return C
}

function subDenseDense(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, A.n)
    }
    for (var i = 0; i < A.m*A.n; i++){
        C.values[i] = A.values[i] - B.values[i]
    }
    return C
}

function addNumberDense(a, B, C){
    if (C === undefined){
        C = new DenseMatrix(B.m, B.n)
    }
    for (var i = 0; i < B.m*B.n; i++){
        C.values[i] = a + B.values[i]
    }
    return C
}

function subNumberDense(a, B, C){
    if (C === undefined){
        C = new DenseMatrix(B.m, B.n)
    }
    for (var i = 0; i < B.m*B.n; i++){
        C.values[i] = a - B.values[i]
    }
    return C
}

DenseMatrix.prototype.dot = function(B, C){ return dot(this, B, C) }
DenseMatrix.prototype.add = function(B, C){ return add(this, B, C) }
DenseMatrix.prototype.sub = function(B, C){ return sub(this, B, C) }

DenseMatrix.prototype.copy = function(){
    var C = new DenseMatrix(this.m, this.n)
    for (var i = 0; i < this.m*this.n; i++){
        C.values[i] = this.values[i]
    }
    return C
}

DenseMatrix.prototype.sum2 = function(B){
    if (B === undefined) B = this
    var A = this
    var sum = 0
    for (var i = 0; i < A.m*A.n; i++){
        sum += A.values[i]*B.values[i]
    }
    return sum
}

// this = scale_a*A + scale_b*Bs
DenseMatrix.prototype.setToProduct = function(scale_a, A, scale_b, B){
    for (var i = 0; i < A.m*A.n; i++){
        this.values[i] = scale_a*A.values[i] + scale_b*B.values[i]
    }
}

DenseMatrix.prototype.getBlock = function(i0, j0, m, n){
    var block = new DenseMatrix(m, n)
    for (var i = 0; i < m; i++){
        for (var j = 0; j < n; j++){
            block.set(i, j, this.get(i + i0, j + j0))
        }
    }
    return block
}

DenseMatrix.prototype.setBlock = function(i0, j0, block){
    for (var i = 0; i < block.m; i++){
        for (var j = 0; j < block.n; j++){
            this.set(i + i0, j + j0, block.get(i, j))
        }
    }
}

// calculates dot(A^T, x) as dot(x^T, A)^T
// without transposing A
function dotT(A, x){
    return x.transpose().dot(A).transpose()
}

DenseMatrix.prototype.print = function(){
    var s = []
    var k = 0
    for (var i = 0; i < this.m; i++){
        for (var j = 0; j < this.n; j++){
            s.push(this.values[k++].toFixed(5))
            s.push(" ")
        }
        s.push("\n")
    }
    console.log(s.join(""))
}

DenseMatrix.prototype.flatten = function(){
    return this.reshape(this.m*this.n, 1)
}

function rand(m, n){
    if (n === undefined){
        n = 1
    }
    
    var A = new DenseMatrix(m, n)
    for (var i = 0; i < m*n; i++){
        A.values[i] = Math.random()
    }
    return A
}

function vals(m, n, value){
    if (n === undefined){
        n = 1
    }
    
    var A = new DenseMatrix(m, n)
    for (var i = 0; i < m*n; i++){
        A.values[i] = value
    }
    return A
}

function ones(m, n){
    return vals(m, n, 1)
}

function zeros(m, n){
    return vals(m, n, 0)
}
