import numpy as np
import scipy.signal
import scipy.fft

def next_power_of_two(n):
    if n <= 1: return 1
    return 1 << (n - 1).bit_length()

def fftconv(x, y):
    nx = len(x)
    ny = len(y)

    m = nx + ny - 1

    n = next_power_of_two(m + 1)

    x = np.fft.rfft(x, n=n)
    y = np.fft.rfft(y, n=n)
    z = x * y
    z = np.fft.irfft(z)
    z = z[:m]

    return z

def main():
    for nx in range(1, 100):
        for ny in range(1, 100):
            x = np.random.rand(nx)
            y = np.random.rand(ny)

            expected = scipy.signal.convolve(x, y, mode='full', method='direct')

            result = fftconv(x, y)

            mse = np.mean(np.square(result - expected))

            assert(mse < 1e-10)

if __name__ == "__main__":
    main()
