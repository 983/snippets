function leastSquares(A, b, max_iterations, tolerance){
    if (max_iterations === undefined){
        throw "please specify maximum number of iterations"
    }
    
    if (tolerance === undefined){
        tolerance = 1e-10
    }
    
    var x = new DenseMatrix(A.n, 1)
    
    var residual = dotT(A, b).sub(dotT(A, dot(A, x)))
    
    var p = residual.copy()
    
    var residual_old = residual.sum2()
     
    for (var iteration = 0; iteration < max_iterations; iteration++){
        console.log("leastSquares iteration", iteration)
        
        var q = dotT(A, dot(A, p))
        
        var alpha = residual_old / p.sum2(q)
        
        x.setToProduct(1, x, alpha, p)
        residual.setToProduct(1, residual, -alpha, q)
        
        var residual_new = residual.sum2()
        
        if (residual_new < tolerance){
            //console.log("below tolerance after", i, "iterations")
            break
        }
        
        p.setToProduct(1, residual, residual_new/residual_old, p)
        
        residual_old = residual_new
    }
    
    return x;
}
