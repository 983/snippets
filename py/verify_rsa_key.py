import base64
import hashlib
import binascii
from fractions import gcd

def lcm(a, b):
    # lowest common multiple
    return (a * b) // gcd(a, b)

def egcd(a, b):
    # extended greatest common denominator
    
    if a == 0:
        return (b, 0, 1)
    
    g, y, x = egcd(b % a, a)
    
    return (g, x - (b // a) * y, y)

def modinv(a, m):
    # modular inverse
    g, x, y = egcd(a, m)
    
    if g != 1:
        raise Exception("Modular inverse of", a, "and", b, "does not exist")
    
    return x % m

key1 = "".join("""
-----BEGIN RSA PRIVATE KEY-----
MIICXAIBAAKBgQCqGKukO1De7zhZj6+H0qtjTkVxwTCpvKe4eCZ0FPqri0cb2JZfXJ/DgYSF6vUp
wmJG8wVQZKjeGcjDOL5UlsuusFncCzWBQ7RKNUSesmQRMSGkVb1/3j+skZ6UtW+5u09lHNsj6tQ5
1s1SPrCBkedbNf0Tp0GbMJDyR4e9T04ZZwIDAQABAoGAFijko56+qGyN8M0RVyaRAXz++xTqHBLh
3tx4VgMtrQ+WEgCjhoTwo23KMBAuJGSYnRmoBZM3lMfTKevIkAidPExvYCdm5dYq3XToLkkLv5L2
pIIVOFMDG+KESnAFV7l2c+cnzRMW0+b6f8mR1CJzZuxVLL6Q02fvLi55/mbSYxECQQDeAw6fiIQX
GukBI4eMZZt4nscy2o12KyYner3VpoeE+Np2q+Z3pvAMd/aNzQ/W9WaI+NRfcxUJrmfPwIGm63il
AkEAxCL5HQb2bQr4ByorcMWm/hEP2MZzROV73yF41hPsRC9m66KrheO9HPTJuo3/9s5p+sqGxOlF
L0NDt4SkosjgGwJAFklyR1uZ/wPJjj611cdBcztlPdqoxssQGnh85BzCj/u3WqBpE2vjvyyvyI5k
X6zk7S0ljKtt2jny2+00VsBerQJBAJGC1Mg5Oydo5NwD6BiROrPxGo2bpTbu/fhrT8ebHkTz2epl
U9VQQSQzY1oZMVX8i1m5WUTLPz2yLJIBQVdXqhMCQBGoiuSoSjafUhV7i1cEGpb88h5NBYZzWXGZ
37sJ5QsW+sJyoNde3xH8vdXhzU7eT82D6X/scw9RZz+/6rCJ4p0=
-----END RSA PRIVATE KEY-----
""".strip().split("\n")[1:-1])

key2 = "".join("""
3082025c02010002818100aa18aba43b50deef38598faf87d2ab634e4571c130a9bca7b878267414
faab8b471bd8965f5c9fc3818485eaf529c26246f3055064a8de19c8c338be5496cbaeb059dc0b35
8143b44a35449eb264113121a455bd7fde3fac919e94b56fb9bb4f651cdb23ead439d6cd523eb081
91e75b35fd13a7419b3090f24787bd4f4e196702030100010281801628e4a39ebea86c8df0cd1157
2691017cfefb14ea1c12e1dedc7856032dad0f961200a38684f0a36dca30102e2464989d19a80593
3794c7d329ebc890089d3c4c6f602766e5d62add74e82e490bbf92f6a482153853031be2844a7005
57b97673e727cd1316d3e6fa7fc991d4227366ec552cbe90d367ef2e2e79fe66d26311024100de03
0e9f8884171ae90123878c659b789ec732da8d762b26277abdd5a68784f8da76abe677a6f00c77f6
8dcd0fd6f56688f8d45f731509ae67cfc081a6eb78a5024100c422f91d06f66d0af8072a2b70c5a6
fe110fd8c67344e57bdf2178d613ec442f66eba2ab85e3bd1cf4c9ba8dfff6ce69faca86c4e9452f
4343b784a4a2c8e01b0240164972475b99ff03c98e3eb5d5c741733b653ddaa8c6cb101a787ce41c
c28ffbb75aa069136be3bf2cafc88e645face4ed2d258cab6dda39f2dbed3456c05ead0241009182
d4c8393b2768e4dc03e818913ab3f11a8d9ba536eefdf86b4fc79b1e44f3d9ea6553d55041243363
5a193155fc8b59b95944cb3f3db22c9201415757aa13024011a88ae4a84a369f52157b8b57041a96
fcf21e4d058673597199dfbb09e50b16fac272a0d75edf11fcbdd5e1cd4ede4fcd83e97fec730f51
673fbfeab089e29d
""".strip().split("\n"))

key1 = base64.b64decode(key1)
key2 = binascii.unhexlify(key2)
assert(key1 == key2)

key3 = "".join("""
MIIBOQIBAAJBAIOLepgdqXrM07O4dV/nJ5gSA12jcjBeBXK5mZO7Gc778HuvhJi+
RvqhSi82EuN9sHPx1iQqaCuXuS1vpuqvYiUCAwEAAQJATRDbCuFd2EbFxGXNxhjL
loj/Fc3a6UE8GeFoeydDUIJjWifbCAQsptSPIT5vhcudZgWEMDSXrIn79nXvyPy5
BQIhAPU+XwrLGy0Hd4Roug+9IRMrlu0gtSvTJRWQ/b7m0fbfAiEAiVB7bUMynZf4
SwVJ8NAF4AikBmYxOJPUxnPjEp8D23sCIA3ZcNqWL7myQ0CZ/W/oGVcQzhwkDbck
3GJEZuAB/vd3AiASmnvOZs9BuKgkCdhlrtlM6/7E+y1p++VU6bh2+mI8ZwIgf4Qh
u+zYCJfIjtJJpH1lHZW+A60iThKtezaCk7FiAC4=
""".strip().split("\n"))
key3 = base64.b64decode(key3)

def read_length():
    global i

    if key[i] <= 0x7f:
        n_bytes = key[i]
        i += 1
    elif key[i] == 0x81:
        i += 1
        n_bytes = key[i]
        i += 1
    elif key[i] == 0x82:
        i += 1
        n_bytes = key[i]
        i += 1
        n_bytes = (n_bytes << 8) | key[i]
        i += 1
    else:
        print("TODO: handle length encoding " + hex(key[i]))
        raise NotImplemented("TODO: handle length encoding")

    return n_bytes

def read_int():
    global i

    assert(key[i] == 0x2)
    i += 1

    n_bytes = read_length()
    
    value = key[i:i+n_bytes]
    i += n_bytes

    value = int.from_bytes(value, byteorder='big')

    return value

keys = [key1, key2, key3]

for key in keys:
    # based on https://crypto.stackexchange.com/a/21107
    i = 0

    # sequence tag
    assert(key[i] == 0x30)
    i += 1

    sequence_length = read_length()
    assert(i + sequence_length == len(key))

    version = read_int()
    assert(version == 0)

    n = read_int() # public modulus
    e = read_int() # public exponent
    d = read_int() # private exponent
    p = read_int() # secret prime
    q = read_int() # secret prime
    dp = read_int() # d mod (p - 1)
    dq = read_int() # d mod (q - 1)
    qinv = read_int() # q^-1 mod p

    if 0:
        print("public_modulus_n:")
        print(n)
        print("public_exponent_e:")
        print(e)
        print("private_exponent_d:")
        print(d)
        print("secret_prime_p:")
        print(p)
        print("secret_prime_q:")
        print(q)
        print("dp:")
        print(dp)
        print("dq:")
        print(dq)
        print("qinv:")
        print(qinv)
        print("")

    # those properties should hold
    assert(n == p*q)
    assert((e * d) % lcm(p - 1, q - 1) == 1)
    assert(dp == d % (p - 1))
    assert(dq == d % (q - 1))
    assert(dp == modinv(e, p - 1))
    assert(dq == modinv(e, q - 1))
    assert(i == len(key))
