import numpy as np
import sounddevice as sd
import time, sys, os
import matplotlib.pyplot as plt

samplerate = 48000
block_size = 1000
frequency = 1000
buffer_size = block_size*2

buffer = np.zeros(buffer_size, dtype=np.float32)

t = np.linspace(0, 1, samplerate, endpoint=False)
t = t[:block_size]

data = np.cos(2*np.pi*t*frequency)

x = np.linspace(-3, 3, len(data))
window = np.exp(-x**2)

data *= window

def bandpass(data, low, high):
    data = np.fft.rfft(data)
    data[:low] = 0
    data[high:] = 0
    data = np.fft.irfft(data)
    return data

def fftcorrelate(a, b):
    a = np.fft.rfft(a)
    b = np.fft.rfft(b)
    c = a * b
    c = np.fft.irfft(c)
    return c

a = sd.rec(samplerate, samplerate, channels=1)[:, 0]


