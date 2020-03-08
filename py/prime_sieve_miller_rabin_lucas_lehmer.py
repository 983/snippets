import time, math, random, numpy as np

def is_even(n):
    return (n & 1) == 0

def ctz(n):
    c = 0
    while n > 0 and is_even(n):
        n >>= 1
        c += 1
    return c

def miller_rabin(n, n_tests):
    if n == 2: return True
    if n < 2: return False
    if n % 2 == 0: return False

    d = n - 1
    r = ctz(d)
    d >>= r

    for a in [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41] + [random.randint(2, n - 1) for _ in range(n_tests)]:
        if a >= n: continue

        x = pow(a, d, n)

        if x == 1 or x == n - 1:
            continue

        for iteration in range(r - 1):
            x = pow(x, 2, n)
            
            if x == n - 1:
                break
        else:
            return False

    return True

def make_primes(n):
    is_prime = np.ones(n, dtype=np.bool8)
    is_prime[:2] = False
    for i in range(2, n):
        if i * i >= n: break
        is_prime[i*2::i] = False
    return is_prime

def lucas_lehmer(p):
    if p <= 1: return False
    if p == 2: return True
    s = 4
    m = 2**p - 1
    t = time.perf_counter()
    for iteration in range(p - 2):
        if time.perf_counter() - t > 1.0:
            percent = (iteration + 1) * 100.0 / (p - 2)
            print("%6.3f %%" % percent)
            t = time.perf_counter()
        s *= s
        s = (s & m) + (s >> p)
        if s >= m:
            s -= m
        s -= 2
    return s == 0

def main():
    n = 1000
    
    is_prime = make_primes(n)

    for i in range(n):
        a = is_prime[i]
        b = miller_rabin(i, 3)
        assert(a == b)

    primes = []

    for p in range(100000):
        if not miller_rabin(p, 10): continue
        
        a = lucas_lehmer(p)
        
        b = miller_rabin(2**p - 1, 3)
        
        assert(a == b)

        if a:
            print("Prime: 2**%d - 1" % p)

if __name__ == "__main__":
    main()
