function DiagonalMatrix(diagonal, offset, m, n){
    if (offset === undefined){
        offset = 0
    }
    
    if (m === undefined){
        m = diagonal.m + Math.abs(offset)
    }
    
    if (n === undefined){
        n = m
    }
    
    this.diagonal = diagonal
    this.offset = offset
    this.m = m
    this.n = n
}

DiagonalMatrix.prototype.transpose = function(){
    return new DiagonalMatrix(this.diagonal, -this.offset, this.n, this.m)
}

DiagonalMatrix.prototype.toDense = function(){
    var m = this.m
    var n = this.n
    
    var C = new DenseMatrix(m, n)
    
    var diagonal = this.diagonal
    
    var offset = this.offset
    
    if (offset >= 0){
        for (var k = 0; k < diagonal.m; k++){
            var i = k
            var j = k + offset
            C.values[i*n + j] = diagonal.values[k]
        }
    }else{
        for (var k = 0; k < diagonal.m; k++){
            var i = k - offset
            var j = k
            C.values[i*n + j] = diagonal.values[k]
        }
    }
    
    return C
}

function dotDiagDense(A, B, C){
    var offset = A.offset
    var d = A.diagonal
    
    if (C === undefined){
        C = new DenseMatrix(A.m, B.n)
    }
    
    if (offset >= 0){
        for (var i = 0; i < C.m - offset; i++){
            for (var j = 0; j < C.n; j++){
                C.set(i, j, d.values[i] * B.get(i + offset, j))
            }
        }
    }else{
        for (var i = -offset; i < C.m; i++){
            for (var j = 0; j < C.n; j++){
                C.set(i, j, d.values[i + offset] * B.get(i + offset, j))
            }
        }
    }
    
    return C
}

function dotDenseDiag(A, B){
    var offset = B.offset
    var d = B.diagonal
    
    var C = new DenseMatrix(A.m, B.n)
    
    if (offset >= 0){
        for (var i = 0; i < C.m; i++){
            for (var j = offset; j < C.n; j++){
                C.set(i, j, A.get(i, j - offset) * d.values[j - offset])
            }
        }
    }else{
        for (var i = 0; i < C.m; i++){
            for (var j = 0; j < C.n + offset; j++){
                C.set(i, j, A.get(i, j - offset) * d.values[j])
            }
        }
    }
    
    return C
}

function dotDiagDiag(A, B){
    if (A.m != A.n || B.m != B.n){
        console.log("TODO implement dotDiagDiag for non-square matrices")
        STOP
    }
    
    var na = A.diagonal.m
    var nb = B.diagonal.m
    var da = A.diagonal.values
    var db = B.diagonal.values
    
    var m = A.m
    var n = B.n
    
    var offset = A.offset + B.offset
    var diagonal_length = m - Math.abs(offset)
    
    // diagonal beyond bounds of matrix
    if (diagonal_length <= 0){
        // TODO ZeroMatrix
        var diagonal = new DenseMatrix(m, 1)
        return new DiagonalMatrix(diagonal, 0, m, m)
    }
    
    var diagonal = new DenseMatrix(m, 1)
    var d = diagonal.values
    
    // TODO there is probably a nice way to unify the following if-block
    
    if (A.offset < 0 && B.offset >= 0){
        var n0 = Math.min(na, nb)
        var delta = diagonal_length - n0
        for (var k = 0; k < n0; k++){
            d[k + delta] = da[k]*db[k]
        }
    }else if (A.offset >= 0 && B.offset < 0){
        if (na < nb){
            var delta = nb - na
            for (var k = 0; k < na; k++){
                d[k] = da[k]*db[k + delta]
            }
        }else{
            var delta = na - nb
            for (var k = 0; k < nb; k++){
                d[k] = da[k + delta]*db[k]
            }
        }
    }else{
        var delta_a = Math.max(0, -B.offset)
        var delta_b = Math.max(0, A.offset)
        var n = Math.min(na - delta_a, nb - delta_b)
        
        for (var k = 0; k < n; k++){
            d[k] = da[k + delta_a]*db[k + delta_b]
        }
    }
    
    return new DiagonalMatrix(diagonal, offset, m, m)
}
