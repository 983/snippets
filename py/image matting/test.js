
function testDotDiag(){
    for (var k = 0; k < 50; k++){
        var offset = randi(10) - 5
        var diagonal = rand(10)
        var A0 = new DiagonalMatrix(diagonal, offset)
        var A1 = A0.toDense()
        var B = rand(A1.n, randi(5) + 1)
        
        // dotDiagDense
        assertSmall(sub(dot(A0, B), dot(A1, B)).sum2())
        // dotDenseDiag
        assertSmall(sub(dot(B.transpose(), A0.transpose()), dot(B.transpose(), A1.transpose())).sum2())
    }
    
    for (var k = 0; k < 50; k++){
        var d0 = rand(randi(5) + 1)
        var d1 = rand(randi(5) + 1)
        var offset0 = randi(5) - 2
        var offset1 = randi(5) - 2
        var A0 = new DiagonalMatrix(d0, offset0)
        var B0 = new DiagonalMatrix(d1, offset1)
        
        // keep rolling until sizes for dot product work out
        if (A0.n != B0.m){
            k--
            continue
        }
        
        var A1 = A0.toDense()
        var B1 = B0.toDense()
        
        assertSmall(sub(dot(A0, B0).toDense(), dot(A1, B1)).sum2())
        assertSmall(sub(dot(A0.transpose(), B0.transpose()).toDense(), dot(A1.transpose(), B1.transpose())).sum2())
    }
    
    /*
    var d = new DenseMatrix(3, 1, [1, 2, 3])
    var A = new DiagonalMatrix(d)
    var B = new DenseMatrix(3, 1, [4, 5, 6])
    var C0 = dot(A, B)
    var C1 = dot(B.transpose(), A.transpose()).transpose()
    var D = C0.sub(C1)
    var error = D.sum2()
    assertSmall(error)
    
    for (var k = 0; k < 50; k++){
        var offset = randi(10) - 5
        var diagonal = rand(10)
        var A0 = new DiagonalMatrix(diagonal, offset)
        var A1 = A0.toDense()
        var B = rand(A1.n)
        assertSmall(sub(dot(A0, B), dot(A1, B)).sum2())
        assertSmall(sub(dot(B.transpose(), A0.transpose()), dot(B.transpose(), A1.transpose())).sum2())
    }
    for (var k = 0; k < 50; k++){
        var offset = randi(10) - 5
        var diagonal = rand(10)
        var A0 = new DiagonalMatrix(diagonal, offset)
        var A1 = A0.toDense()
        var B = new DiagonalMatrix(ones(A1.n))
        assertSmall(sub(dot(A0, B), dot(A1, B)).sum2())
        assertSmall(sub(dot(B.transpose(), A0.transpose()), dot(B.transpose(), A1.transpose())).sum2())
    }
    */
}

testDotDiag()

function testLeastSquares(){
    for (var i = 0; i < 10; i++){   
        var n = 20
        var A = rand(n, n)
        var b = rand(n, 1)
        
        var x = leastSquares(A, b, n*3)
        
        var d = A.dot(x).sub(b)
        var error = d.sum2()
        
        assertSmall(error)
    }
}

testLeastSquares()

function testDotBlock(){
    var A = rand(7, 8)
    var block = rand(2, 3)
    var B0 = new BlockMatrix(block, 4, 5, 8, 9)
    var B1 = new DenseMatrix(8, 9)
    B1.setBlock(4, 5, block)
    
    var error = dot(A, B0).sub(dot(A, B1)).sum2()
    assertSmall(error)
    
    A = A.transpose()
    B0 = B0.transpose()
    B1 = B1.transpose()
    
    var error = dot(B0, A).sub(dot(B1, A)).sum2()
    assertSmall(error)
}

testDotBlock()

function testDotSum(){  
    var n = 10
    var A0 = rand(n, n)
    var A1 = rand(n, n)
    var A = new SumMatrix([A0, A1], n, n)
    var B = rand(n, n)
    var C0 = dotSumDense(A, B)
    var C1 = dot(A0.add(A1), B)
    assertSmall(C0.sub(C1).sum2())
}

testDotSum()

console.log("Tests passed", new Date())
