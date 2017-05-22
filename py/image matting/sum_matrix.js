function SumMatrix(matrices, m, n){
    assertNumber(m)
    assertNumber(n)
    
    this.matrices = matrices
    this.m = m
    this.n = n
}

function dotSumDense(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, B.n)
    }
    
    for (var k = 0; k < A.matrices.length; k++){
        var temp = dot(A.matrices[k], B)
        
        if (temp.constructor !== DenseMatrix){
            throw ["TODO implement", temp.constructor]
        }
        
        for (var i = 0; i < temp.m*temp.n; i++){
            C.values[i] += temp.values[i]
        }
    }
    
    return C
}

function dotDenseSum(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, B.n)
    }
    
    for (var k = 0; k < B.matrices.length; k++){
        var temp = dot(A, B.matrices[k])
        
        if (temp.constructor !== DenseMatrix){
            throw ["TODO implement", temp.constructor]
        }
        
        for (var i = 0; i < temp.m*temp.n; i++){
            C.values[i] += temp.values[i]
        }
    }
    
    return C
}

SumMatrix.prototype.transpose = function(){
    var matrices = new Array(this.matrices.length)
    for (var i = 0; i < matrices.length; i++){
        matrices[i] = this.matrices[i].transpose()
    }
    return new SumMatrix(matrices, this.n, this.m)
}
