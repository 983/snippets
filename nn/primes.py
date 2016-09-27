import math

def is_prime(x):
    if x & 1 == 0:
        return False
    
    n = 3
    while n*n <= x:
        if x % n == 0:
            return False
        n += 2
    
    return True

primes = [n for n in range(1000) if is_prime(n)]

def print_4x4(offset):
    rows = []
    for i in range(offset, offset + 4):
        #print(str(primes[i*4:i*4+4])[1:-1] + ",")
        rows.append("{" + str(primes[i*4:i*4+4])[1:-1] + "}")
    print("{" + ",".join(rows) + "}")

print_4x4(0)
print_4x4(4)
