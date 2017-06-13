import numpy as np

def fftcorrelate(a, b):
    a = np.fft.rfft(a)
    b = np.fft.rfft(b)
    c = a * b
    c = np.fft.irfft(c)
    return c

def lerp(a, b, u):
    return a*(1 - u) + u*b

def tukey(M, alpha=0.5, sym=True):
    n = np.arange(0, M)
    width = int(np.floor(alpha*(M-1)/2.0))
    n1 = n[0:width+1]
    n2 = n[width+1:M-width-1]
    n3 = n[M-width-1:]

    w1 = 0.5 * (1 + np.cos(np.pi * (-1 + 2.0*n1/alpha/(M-1))))
    w2 = np.ones(n2.shape)
    w3 = 0.5 * (1 + np.cos(np.pi * (-2.0/alpha + 1 + 2.0*n3/alpha/(M-1))))

    w = np.concatenate((w1, w2, w3))

    return w

samplerate  = 48000
block_size  = 200
buffer_size = 2000

f0 = 12*1000*block_size/float(samplerate)
f1 = 16*1000*block_size/float(samplerate)

t = np.linspace(0, 1, block_size, endpoint=False)

chirp = np.cos(2*np.pi*t*lerp(f0, f1, t))

window = tukey(len(chirp))

data = np.zeros(buffer_size)
data[:len(chirp)] = chirp * window
