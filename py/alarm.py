import sounddevice as sd
import time
import math
import numpy as np
import sys

samplerate = 44100

def make_sine(seconds, frequency):
    t = np.linspace(0, seconds, samplerate*seconds, endpoint=False)
    return np.sin(2.0*np.pi*frequency*t)

sound = np.concatenate([make_sine(0.5, 1000), make_sine(0.5, 500)])

hours, minutes, seconds = list(map(float, sys.argv[1:]))

wait_seconds = hours*60.0*60.0 + minutes*60.0 + seconds

while wait_seconds > 0.0:
    sleep_seconds = min(wait_seconds, 1.0)
    time.sleep(sleep_seconds)
    wait_seconds -= sleep_seconds

    hours = math.floor(wait_seconds/60.0/60.0)
    minutes = math.floor((wait_seconds - hours*60.0*60.0)/60.0)
    seconds = wait_seconds - hours*60.0*60.0 - minutes*60.0
    
    print("%f hours, %f minutes, %f seconds"%(hours, minutes, seconds))

sd.play(sound, samplerate=samplerate, loop=True)
sd.wait()
