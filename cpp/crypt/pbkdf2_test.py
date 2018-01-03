import hashlib
import binascii
import time

iterations = 4096
key = bytes([1, 2, 3])
salt = bytes([4, 5, 6, 7])

dklen = 256
hash_func = hashlib.sha256

t = time.clock()

derived_key = hashlib.pbkdf2_hmac('sha256', key, salt, iterations, dklen=dklen)

dt = time.clock() - t

print("%f seconds"%dt)

blocksize = 64

if len(key) > blocksize:
    key = hash_func(key).digest()

block = key + b'\0' * (blocksize - len(key))

dkey = b''
loop = 1
while len(dkey) < dklen:
    u = salt + loop.to_bytes(4, 'big')
    rkey = b'\0'*hash_func().digest_size

    for i in range(iterations):
        u = hash_func(bytes(x ^ 0x36 for x in block) + u).digest()
        u = hash_func(bytes(x ^ 0x5c for x in block) + u).digest()
        
        rkey = bytes(a^b for a,b in zip(rkey, u))
    
    loop += 1
    dkey += rkey

assert(derived_key == dkey[:dklen])
print("success!")
print(list(derived_key))
