import math
from gmpy2 import mpq, mpz

def mod1(x):
    return x - mpz(x)

def pi():
    # https://web.archive.org/web/20150627225748/http://en.literateprograms.org/Pi_with_the_BBP_formula_%28Python%29
    x = 0
    n = 1
    while 1:
        p = mpq((120*n - 89)*n + 16, (((512*n - 1024)*n + 712)*n - 206)*n + 21)
        x = mod1(16*x + p)
        n += 1
        yield int(16*x)

def make_pi_digits1(n_decimal_digits):
    n_hexadecimal_digits = math.ceil(n_decimal_digits*math.log(10)/math.log(16))
    
    x = 3
    p = pi()
    for _ in range(n_hexadecimal_digits):
        x = x*16 + next(p)
    
    hexadecimal_digits = x
    decimal_digits = hexadecimal_digits * 10**n_decimal_digits >> (n_hexadecimal_digits * 4)
    
    return str(decimal_digits)

def calcPi():
    q, r, t, k, n, l = 1, 0, 1, 1, 3, 3
    while True:
        if 4*q+r-t < n*t:
            yield n
            nr = 10*(r-n*t)
            n  = ((10*(3*q+r))//t)-10*n
            q  *= 10
            r  = nr
        else:
            nr = (2*q+r)*l
            nn = (q*(7*k)+2+(r*l))//(t*l)
            q  *= k
            t  *= l
            l  += 2
            k += 1
            n  = nn
            r  = nr

def make_pi_digits2(n_decimal_digits):
    pi_digits = calcPi()
    return "".join("%d"%next(pi_digits) for _ in range(n_decimal_digits + 1))

print("31415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679")
print(make_pi_digits1(100))
print(make_pi_digits2(100))
