import numpy as np
import matplotlib.pyplot as plt
import scipy.signal
import time, sys, os

n = 1024
t = np.linspace(0, 1, n, endpoint=False)

n_chirps = 4
noise = np.random.randn(n*n_chirps)

f0 = 400.0
f1 = 450.0

window_names = [
    "boxcar",
    "triang",
    "blackman",
    "hamming",
    "hann",
    "bartlett",
    "flattop",
    "parzen",
    "bohman",
    "blackmanharris",
    "nuttall",
    "barthann",
]

windows = [
    scipy.signal.tukey(n),
    #scipy.signal.general_gaussian(n, p=1.5, sig=7),
    #scipy.signal.gaussian(n, std=7),
    scipy.signal.kaiser(n, beta=14),
    scipy.signal.slepian(n, width=0.3),
    scipy.signal.chebwin(n, 100),
    #scipy.signal.exponential(n, tau=3.0),
]

methods = ['linear', 'quadratic', 'logarithmic', 'hyperbolic']

if 0:
    # show windows
    for i, window in enumerate(windows):
        plt.subplot(len(windows), 1, 1 + i)
        plt.plot(window)
    plt.show()
    sys.exit(0)

def make_plot(method, f0, f1, window, label):
    s = scipy.signal.chirp(t, f0, 1.0, f1, method=method)

    data = np.tile(s*window, n_chirps)
    data += noise

    data = np.correlate(data, s*window, mode='full')
    analytic_signal = scipy.signal.hilbert(data)
    amplitude_envelope = np.abs(analytic_signal)

    plt.plot(amplitude_envelope/np.max(amplitude_envelope), label=label)

for method in methods:
    make_plot(method, f0, f1, windows[0], label=method)
plt.legend()
plt.show()

for i, window_name in enumerate(window_names):
    window = scipy.signal.get_window(window_name, n)
    plt.subplot(len(window_names), 1, 1 + i)
    make_plot(methods[0], f0, f1, window, window_name)
    plt.legend()
plt.show()

for i, window in enumerate(windows):
    plt.subplot(len(windows), 1, 1 + i)
    make_plot(methods[0], f0, f1, window, "window %d"%i)
    plt.legend()
plt.show()
