function swap(a, i, j){
    temp = a[i]
    a[i] = a[j]
    a[j] = temp
}

function nextPowerOfTwo(x){
    x--
    x |= x >> 1
    x |= x >> 2
    x |= x >> 4
    x |= x >> 8
    x |= x >> 16
    x++
    return x
}

function bitlength(x){
    var n = 0
    for (; x > 0; x >>= 1) n++
    return n
}

function bitreverse(x, n){
    var result = 0
    for (var i = 0; i < n; i++){
        var bit = (x >> i) & 1
        result = (result << 1) | bit
    }
    return result
}

function FFT(n){
    var indices = new Int32Array(n)
    var c = new Float64Array(n)
    var s = new Float64Array(n)
    var log2_n = bitlength(n) - 1
    
    for (var i = 0; i < n; i++){
        var angle = -2*Math.PI/n*i
        c[i] = Math.cos(angle)
        s[i] = Math.sin(angle)
        indices[i] = bitreverse(i, log2_n)
    }
    
    this.n = n
    this.log2_n = log2_n
    this.indices = indices
    this.c = c
    this.s = s
}

FFT.prototype.swap = function(real, imag){
    var n = this.n
    var log2_n = this.log2_n
    var indices = this.indices
    
    if (real.length != n || imag.length != n){
        throw "real and imag array must have length " + n + " but are " + real.length + " and " + imag.length
    }
    
    for (var i = 0; i < n; i++){
        var j = indices[i]
        
        if (i < j){
            swap(real, i, j)
            swap(imag, i, j)
        }
    }
}

FFT.prototype.fft = function(real, imag){
    var c = this.c
    var s = this.s
    var n = this.n
    
    this.swap(real, imag)
    
    for (var block_size = 2; block_size <= n; block_size <<= 1){
        var cos_sin_stride = n / block_size
        
        for (var block_offset = 0; block_offset < n; block_offset += block_size){
            
            var half_block_size = block_size >> 1
            
            for (var i = 0; i < half_block_size; i++){
                ar = real[block_offset + i]
                ai = imag[block_offset + i]
                
                br = real[block_offset + i + half_block_size]
                bi = imag[block_offset + i + half_block_size]
                
                cr = c[i*cos_sin_stride]
                ci = s[i*cos_sin_stride]
                
                dr = br*cr - bi*ci
                di = br*ci + bi*cr
                
                real[block_offset + i                  ] = ar + dr
                imag[block_offset + i                  ] = ai + di
                real[block_offset + i + half_block_size] = ar - dr
                imag[block_offset + i + half_block_size] = ai - di
            }
        }
    }
}

FFT.prototype.ifft = function(real, imag){
    // only difference to fft is sign and scale
    var c = this.c
    var s = this.s
    var n = this.n
    
    this.swap(real, imag)
    
    for (var block_size = 2; block_size <= n; block_size <<= 1){
        var cos_sin_stride = n / block_size
        
        for (var block_offset = 0; block_offset < n; block_offset += block_size){
            
            var half_block_size = block_size >> 1
            
            for (var i = 0; i < half_block_size; i++){
                ar = real[block_offset + i]
                ai = imag[block_offset + i]
                
                br = real[block_offset + i + half_block_size]
                bi = imag[block_offset + i + half_block_size]
                
                cr =  c[i*cos_sin_stride]
                // sign
                ci = -s[i*cos_sin_stride]
                
                dr = br*cr - bi*ci
                di = br*ci + bi*cr
                
                real[block_offset + i                  ] = ar + dr
                imag[block_offset + i                  ] = ai + di
                real[block_offset + i + half_block_size] = ar - dr
                imag[block_offset + i + half_block_size] = ai - di
            }
        }
    }
}

FFT.prototype.scale = function(real, imag){
    var n = this.n
    var scale = 1.0/n
    for (var i = 0; i < n; i++){
        real[i] *= scale
        imag[i] *= scale
    }
}
