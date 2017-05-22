function BlockMatrix(block, i0, j0, m, n){
    this.block = block
    this.i0 = i0
    this.j0 = j0
    this.m = m
    this.n = n
}

function dotBlockDense(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, B.n)
    }
    
    var B_block = B.getBlock(A.j0, 0, A.block.n, B.n)
    
    C.setBlock(A.i0, 0, dot(A.block, B_block))
    
    return C
}

function dotDenseBlock(A, B, C){
    if (C === undefined){
        C = new DenseMatrix(A.m, B.n)
    }
    
    var A_block = A.getBlock(0, B.i0, A.m, B.block.m)
    
    C.setBlock(0, B.j0, dot(A_block, B.block))
    
    return C
}

BlockMatrix.prototype.transpose = function(){
    return new BlockMatrix(this.block.transpose(), this.j0, this.i0, this.n, this.m)
}
