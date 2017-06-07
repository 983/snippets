import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import sounddevice as sd
import time, sys, os

width = 512
height = 512

samplerate = 48000
blocksize = 1024
frequency = 6000
buffer_size = blocksize*2

buffer = np.zeros(buffer_size, dtype=np.float32)

t = np.linspace(0, 1, samplerate, endpoint=False)
t = t[:blocksize]

data = np.cos(2*np.pi*t*frequency)

x = np.linspace(-3, 3, len(data))
window = np.exp(-x**2)

data *= window

def callback(indata, frames, time, status):
    buffer[:blocksize] = indata[:, 0]
    buffer[:] = np.roll(buffer, -blocksize)

stream = sd.InputStream(
    channels=1,
    callback=callback,
    blocksize=blocksize,
    samplerate=samplerate)

stream.start()

if 1:
    pygame.init()
    display = (width, height)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    while True:
        for event in pygame.event.get():
            if (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE) or event.type == pygame.QUIT:
                stream.stop()
                pygame.display.quit()
                pygame.quit()
                sys.exit(0)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        result = np.abs(np.correlate(buffer, data, mode='same'))

        glBegin(GL_LINE_STRIP)
        for x, y in zip(np.linspace(-1, 1, len(result)), result):
            glVertex2f(x, y*2.0/np.sqrt(len(buffer)))
        glEnd()
        
        pygame.display.flip()
        pygame.time.wait(10)

"""
import sounddevice as sd
from numpy import *
import matplotlib.pyplot as plt
import time, sys, os

def smoothstep(x, a, b):
    t = clip((x - a)/(b - a), 0.0, 1.0)
    return (3.0 - 2.0*t)*t*t

samplerate = 48000
blocksize = 1000

n = samplerate

t = linspace(0, 1, samplerate, endpoint=False)
t = t[:blocksize]

data = cos(2*pi*t*frequency)

x = linspace(-3, 3, len(data))
window = exp(-x**2)

if 0:
    plt.plot(window)
    plt.show()
    sys.exit(0)

data *= window

def callback(outdata, frames, time, status):
    outdata[:] = data.reshape((blocksize, 1))

stream = sd.InputStream(
    samplerate=samplerate,
    blocksize=blocksize,
    dtype=float32,
    channels=1,
    callback=callback)

stream.start()
time.sleep(2)
stream.stop()
print("done")
"""
