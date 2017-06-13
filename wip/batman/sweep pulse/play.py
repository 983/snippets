import sounddevice as sd
import numpy as np
import scipy.signal
import time, sys, os

def lerp(a, b, u):
    return a*(1 - u) + u*b

samplerate = 48000
blocksize = 1024

f0 = 14*1000*blocksize/float(samplerate)
f1 = 18*1000*blocksize/float(samplerate)

t = np.linspace(0, 1, blocksize, endpoint=False)

data = scipy.signal.chirp(t, f0, 1.0, f1)

window = scipy.signal.tukey(blocksize)

data = data*window

if 0:
    import matplotlib.pyplot as plt
    plt.plot(data)
    #plt.plot(window)
    plt.show()
    sys.exit(0)

data = data.reshape((len(data), 1))
def callback(outdata, frames, time, status):
    outdata[:] = data

stream = sd.OutputStream(
    samplerate=samplerate,
    blocksize=len(data),
    dtype=np.float32,
    channels=1,
    callback=callback)

stream.start()
# chirp for one hour
time.sleep(60*60)
stream.stop()
