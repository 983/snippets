import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import sounddevice as sd
import time, sys, os
import matplotlib.pyplot as plt
import scipy.signal

def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = scipy.signal.butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, samplerate, order=5):
    b, a = butter_bandpass(lowcut, highcut, samplerate, order=order)
    y = scipy.signal.lfilter(b, a, data)
    return y

width = 512
height = 512

block_size = 1024
buffer_size = 8*1024
samplerate = 48000

buffer = np.zeros(buffer_size, dtype=np.float32)

def callback(indata, frames, time, status):
    buffer[:block_size] = indata[:, 0]
    buffer[:] = np.roll(buffer, -block_size)

stream = sd.InputStream(
    channels=1,
    callback=callback,
    blocksize=block_size,
    samplerate=samplerate)

stream.start()

window = scipy.signal.tukey(len(buffer))

if 1:
    pygame.init()
    flags = DOUBLEBUF | OPENGL | pygame.RESIZABLE
    surface = pygame.display.set_mode((width, height), flags)

    while True:
        for event in pygame.event.get():
            if (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE) or event.type == pygame.QUIT:
                stream.stop()
                pygame.display.quit()
                pygame.quit()
                sys.exit(0)

            if event.type == pygame.VIDEORESIZE:
                print(event)
                surface = pygame.display.set_mode((event.w, event.h), flags)
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        #result = np.abs(np.fft.rfft(buffer*window))
        f, result = scipy.signal.periodogram(buffer, fs=buffer_size/float(samplerate))
        
        glBegin(GL_LINE_STRIP)
        for x, y in zip(np.linspace(-1, 1, len(result)), result):
            glVertex2f(x, y*0.5)
        glEnd()
        
        pygame.display.flip()
        pygame.time.wait(10)
