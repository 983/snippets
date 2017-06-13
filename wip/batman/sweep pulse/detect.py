import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import sounddevice as sd
import time, sys, os
import matplotlib.pyplot as plt
from common import *
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

buffer = np.zeros(buffer_size, dtype=np.float32)

def callback(indata, frames, time, status):
    #buffer[:block_size] = indata[:, 0]
    #buffer[:] = np.roll(buffer, -block_size)
    buffer[:] = indata[:, 0]

stream = sd.InputStream(
    channels=1,
    callback=callback,
    blocksize=buffer_size,
    samplerate=samplerate)

stream.start()

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


        if 1:
            slop = 100
            bandpassed_buffer = butter_bandpass_filter(buffer, f0*samplerate/block_size - slop, f1*samplerate/block_size + slop, samplerate)

            result = fftcorrelate(bandpassed_buffer, np.pad(chirp*window, (0, len(bandpassed_buffer) - len(chirp)), mode='constant'))
        else:
            result = fftcorrelate(buffer, np.pad(chirp*window, (0, len(buffer) - len(chirp)), mode='constant'))

        analytic_signal = scipy.signal.hilbert(result)
        amplitude_envelope = np.abs(analytic_signal)

        glBegin(GL_LINE_STRIP)
        for x, y in zip(np.linspace(-1, 1, len(result)), amplitude_envelope):
            glVertex2f(x, y*0.5 + 0.25)
        glEnd()

        glBegin(GL_LINE_STRIP)
        for x, y in zip(np.linspace(-1, 1, len(result)), buffer):
            glVertex2f(x, y*4.0 - 0.25)
        glEnd()
        
        pygame.display.flip()
        pygame.time.wait(10)
