from fractions import gcd
import random, time, sys, os

def lcm(a, b):
    # lowest common multiple
    return (a * b) // gcd(a, b)

def modular_inverse(a, n):
    t = 0
    new_t = 1
    r = n
    new_r = a

    while new_r != 0:
        quotient = r // new_r
        t, new_t = (new_t, t - quotient * new_t)
        r, new_r = (new_r, r - quotient * new_r)

    if r > 1:
        raise Exception("a is not invertible")

    if t < 0:
        t += n
    
    return t

def generate_keypair(p, q):
    n = p * q

    phi = lcm(p - 1, q - 1)

    # common value for e
    e = (1 << 16) + 1

    assert(e < phi)
    # e and totient must be coprime
    assert(gcd(e, phi) == 1)

    d = modular_inverse(e, phi)
    assert((e*d) % phi == 1)
    
    public_key = (e, n)
    private_key = (d, n)

    return public_key, private_key

def encrypt(private_key, plaintext):
    key, n = private_key
    return [pow(char, key, n) for char in plaintext]

def decrypt(public_key, ciphertext):
    key, n = public_key
    return bytes([pow(char, key, n) for char in ciphertext])

def check_witness(a, d, n, r):
    x = pow(a, d, n)

    if x == 1 or x == n - 1: return True
    
    for _ in range(r - 1):
        x = pow(x, 2, n)
        
        if x == 1: return False
        if x == n - 1: return True

    return False

def is_probable_prime(n, k=10):
    # https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
    if n == 0: return False
    if n == 1: return False
    if n == 2: return True
    if n == 3: return True
    if (n & 1) == 0: return False

    r = 0
    d = n - 1
    while (d & 1) == 0:
        d >>= 1
        r += 1

    assert((n - 1) == (d << r))

    # [2, 3] are enough witnesses for all numbers below (not equal to) 1373653
    witnesses = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41]

    for witness in witnesses:
        if witness >= n: break
        if not check_witness(witness, d, n, r): return False

    for _ in range(k):
        a = random.randrange(2, n - 1)
        if not check_witness(a, d, n, r): return False
    
    return True

# do some testing to ensure that miller rabin test works
if 1:
    is_prime = [True for _ in range(10*1000)]

    for i in range(2, len(is_prime)):
        multiple = 2*i
        while multiple < len(is_prime):
            is_prime[multiple] = False
            multiple += i

    is_prime[0] = False
    is_prime[1] = False

    for i in range(len(is_prime)):
        if is_prime[i] != is_probable_prime(i):
            print("is_probable_prime error:", i)

    primes = [
        983,
        99991,
        999983,
        9999999943,
        9999999967,
        9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999203,
    ]

    not_primes = [prime + 2 for prime in primes]

    for prime in primes:
        assert(is_probable_prime(prime))

    for not_prime in not_primes:
        assert(not is_probable_prime(not_prime))

def find_large_prime(n_bits):
    rng = random.SystemRandom()
    
    while True:
        x = rng.randrange(1 << n_bits)
        if (x & 1) == 0:
            x += 1

        if is_probable_prime(x):
            return x

if 0:
    # test some vulnerabilities
    for _ in range(10*1000):
        n_bits = 32
        p = find_large_prime(n_bits)
        q = find_large_prime(n_bits)
        public_key, private_key = generate_keypair(p, q)
        e, n = public_key
        d, n = private_key

        n4 = pow(n, 0.25)

        if abs(p - q) < 2*n4: print(p, q, "vulnerable to fermat factorization")
        if d < n4/3: print(p, q, "vulnerable to Michael J. Wiener", q < p < 2*q)

# n_bits should at least be 4096 or higher
# this small number is just for testing
n_bits = 128
p = find_large_prime(n_bits)
q = find_large_prime(n_bits)
# there should be additional checks on p and q
# to ensure they have no properties which make factoring too easy
# for example:
# https://en.wikipedia.org/wiki/RSA_(cryptosystem)#Faulty_key_generation

public_key, private_key = generate_keypair(p, q)

print("public key:")
print(public_key)
print("")

print("private key:")
print(private_key)
print("")

plaintext = b"Hello, World!"
# note that encrypting multiple chars/messages with RSA is a bad idea, see:
# https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_Codebook_.28ECB.29
# it is better to encrypt a key for symmetric encryption
encrypted = encrypt(public_key, plaintext)
decrypted = decrypt(private_key, encrypted)

print("encrypted:")
print(encrypted)
print("")

print("decrypted:")
print(decrypted)
print("")

