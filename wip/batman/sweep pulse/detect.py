import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import sounddevice as sd
import time, sys, os
import matplotlib.pyplot as plt
from common import *

width = 512
height = 512
buffer_size = block_size*2

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

        #bandpass_width = 100
        #bandpassed_buffer = butter_bandpass_filter(buffer, frequency - bandpass_width, frequency + bandpass_width, samplerate)

        #result = fftcorrelate(bandpassed_buffer, np.pad(data, (0, block_size), mode='constant'))

        result = fftcorrelate(buffer, np.pad(data, (0, block_size), mode='constant'))

        result = np.abs(result)

        glBegin(GL_LINE_STRIP)
        for x, y in zip(np.linspace(-1, 1, len(result)), result):
            glVertex2f(x, y*0.5 + 0.25)
        glEnd()

        glBegin(GL_LINE_STRIP)
        for x, y in zip(np.linspace(-1, 1, len(result)), buffer):
            glVertex2f(x, y*4.0 - 0.25)
        glEnd()
        
        pygame.display.flip()
        pygame.time.wait(10)
