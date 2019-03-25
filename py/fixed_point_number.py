import math
import time
import random

n_frac_bits = 1 << 20
#n_frac_decimal_digits = int(n_frac_bits / math.log2(10))
n_frac_decimal_digits = 20

class Fixed(object):
    
    def __init__(self, value, shift=0):
        self.value = value << (n_frac_bits + shift)

    def __mul__(self, other):
        a = self.value
        b = other.value
        c = a * b  >> n_frac_bits
        return Fixed(c, -n_frac_bits)
    
    def __add__(self, other):
        return Fixed(self.value + other.value, -n_frac_bits)
    
    def __neg__(self):
        return Fixed(-self.value, -n_frac_bits)
    
    def __sub__(self, other):
        return self + (-other)
    
    def __str__(self):
        x = self.value
        
        x *=  10**n_frac_decimal_digits
        x >>= n_frac_bits
        
        s = str(x)
        s = s[:-n_frac_decimal_digits] + "." + s[-n_frac_decimal_digits:]
        
        return s
    
    __repr__ = __str__

# benchmark integer division
# fast if result is small
for _ in range(10):
    t = time.perf_counter()
    
    if 1:
        result = (1 << n_frac_bits + 20) // ((1 << n_frac_bits) >> 4)
    else:
        # result has more bits, therefore slower
        a = random.getrandbits(n_frac_bits)
        b = random.getrandbits(n_frac_bits // 2)
        result = a // b
        result = 0
    
    dt = time.perf_counter() - t
    
    result = result / (1 << 20)
    print("%f %.20f"%(dt, result))

print("")

# benchmark division using newton method with fixed point numbers
for _ in range(10):
    t = time.perf_counter()
    
    # b must be <= 1
    # b = 1 / 2^5
    b = Fixed(1, -4)
    
    def newton_step(x):
        return x*(Fixed(2) - b*x)
    
    x = Fixed(1)
    for _ in range(10):
        x = newton_step(x)
    
    dt = time.perf_counter() - t
    
    print("%f %s"%(dt, x))
