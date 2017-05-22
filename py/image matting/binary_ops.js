function dot(A, B, C){
    if (A.n !== B.m){
        console.log("wrong shape for dot product between", A, "and", B)
        STOP
    }
    
    if (A.constructor === DenseMatrix && B.constructor === DenseMatrix){
        return dotDenseDense(A, B, C)
    }
    if (A.constructor === DenseMatrix && B.constructor === DiagonalMatrix){
        return dotDenseDiag(A, B, C)
    }
    if (A.constructor === DiagonalMatrix && B.constructor === DenseMatrix){
        return dotDiagDense(A, B, C)
    }
    if (A.constructor === DiagonalMatrix && B.constructor === DiagonalMatrix){
        return dotDiagDiag(A, B, C)
    }
    if (A.constructor === BlockMatrix && B.constructor === DenseMatrix){
        return dotBlockDense(A, B, C)
    }
    if (A.constructor === DenseMatrix && B.constructor === BlockMatrix){
        return dotDenseBlock(A, B, C)
    }
    if (A.constructor === SumMatrix && B.constructor === DenseMatrix){
        return dotSumDense(A, B, C)
    }
    if (A.constructor === DenseMatrix && B.constructor === SumMatrix){
        return dotDenseSum(A, B, C)
    }
    
    throw ["TODO implement dot", A.constructor, B.constructor]
}

function add(A, B, C){
    if (A.constructor === Number && B.constructor === DenseMatrix){
        return addNumberDense(A, B, C)
    }
    
    if (A.m !== B.m || A.n !== B.n){
        console.log("wrong shape for add", A.m, A.n, "with", B.m, B.n)
        STOP
    }
    
    if (A.constructor === DenseMatrix && B.constructor === DenseMatrix){
        return addDenseDense(A, B, C)
    }
    throw ["TODO implement add", A.constructor, B.construtor]
}

function sub(A, B, C){
    if (A.constructor === Number && B.constructor === DenseMatrix){
        return subNumberDense(A, B, C)
    }
    
    if (A.m !== B.m || A.n !== B.n){
        console.log("wrong shape for sub", A.m, A.n, "with", B.m, B.n)
        STOP
    }
    
    if (A.constructor === DenseMatrix && B.constructor === DenseMatrix){
        return subDenseDense(A, B, C)
    }
    throw ["TODO implement sub", A.constructor, B.construtor]
}
