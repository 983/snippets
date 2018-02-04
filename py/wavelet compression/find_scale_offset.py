import numpy as np

def find_scale_offset(a, b):
    # find s and c which minimize |s*a + c - b|_2
    n = len(a)
    sa = np.sum(a)
    sb = np.sum(b)
    saa = np.inner(a, a)
    sab = np.inner(a, b)
    s = (n*sab - sa*sb)/(n*saa - sa*sa)
    c = (sb - s*sa)/n
    return s, c

if __name__ == "__main__":
    import scipy.optimize

    def f(coeffs, a, b):
        s, c = coeffs
        d = s*a + c - b
        return np.sum(d*d)

    a = np.random.rand(8)
    b = 1/9*a + 1/3

    result = scipy.optimize.fmin(f, np.zeros(2), args=(a, b))

    print(result)

    s, c = find_scale_offset(a, b)
    print(s)
    print(c)
    print(f((s, c), a, b))

