import sounddevice as sd
from numpy import *
import time, sys, os
    
def smoothstep(x, a, b):
    t = clip((x - a)/(b - a), 0.0, 1.0)
    return (3.0 - 2.0*t)*t*t

samplerate = 48000
blocksize = 1000
frequency = 6000

n = samplerate

t = linspace(0, 1, samplerate, endpoint=False)
t = t[:blocksize]

data = cos(2*pi*t*frequency)

x = linspace(-3, 3, len(data))
window = exp(-x**2)

if 0:
    import matplotlib.pyplot as plt
    plt.subplot(1, 3, 1)
    plt.plot(window)
    plt.subplot(1, 3, 2)
    plt.plot(data)
    plt.subplot(1, 3, 3)
    plt.plot(data*window)
    plt.show()
    sys.exit(0)

data *= window

def callback(outdata, frames, time, status):
    outdata[:] = data.reshape((blocksize, 1))

stream = sd.OutputStream(
    samplerate=samplerate,
    blocksize=blocksize,
    dtype=float32,
    channels=1,
    callback=callback)

stream.start()
# chirp for one hour
time.sleep(60*60)
stream.stop()
