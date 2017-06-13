import sounddevice as sd
import numpy as np
import time, sys, os
from common import *


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
