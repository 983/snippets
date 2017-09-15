from Crypto import Random
from Crypto.Cipher import AES
from ctypes import cdll, c_ubyte, c_size_t
import time

BLOCK_SIZE = 16

rng = Random.new()

dll = cdll.LoadLibrary("aes.dll")

def encrypt_aes128_cbc(plaintext, key, iv):
    n_bytes = len(plaintext)
    
    iv = (c_ubyte*BLOCK_SIZE).from_buffer_copy(iv)
    key = (c_ubyte*BLOCK_SIZE).from_buffer_copy(key)
    encrypted = (c_ubyte*n_bytes).from_buffer_copy(plaintext)
    n_bytes = c_size_t(n_bytes)

    dll.encrypt_aes128_cbc(encrypted, encrypted, n_bytes, key, iv)

    return bytes(encrypted)

def decrypt_aes128_cbc(encrypted, key, iv):
    n_bytes = len(encrypted)
    
    iv = (c_ubyte*BLOCK_SIZE).from_buffer_copy(iv)
    key = (c_ubyte*BLOCK_SIZE).from_buffer_copy(key)
    decrypted = (c_ubyte*n_bytes).from_buffer_copy(encrypted)
    n_bytes = c_size_t(n_bytes)

    dll.decrypt_aes128_cbc(decrypted, decrypted, n_bytes, key, iv)

    return bytes(decrypted)

def encrypt_pycrypto(plaintext, key, iv):
    encrypted = AES.new(key, AES.MODE_CBC, iv).encrypt(plaintext)
    return encrypted

def decrypt_pycrypto(plaintext, key, iv):
    encrypted = AES.new(key, AES.MODE_CBC, iv).decrypt(plaintext)
    return encrypted

for n_blocks in list(range(10)) + [128, 256, 512, 1024, 4096, 65536]:
    for _ in range(10):
        n_bytes = n_blocks*BLOCK_SIZE

        iv  = rng.read(BLOCK_SIZE)
        key = rng.read(BLOCK_SIZE)
        plaintext = rng.read(n_bytes)

        t0 = time.clock()
        encrypted = encrypt_aes128_cbc(plaintext, key, iv)
        t1 = time.clock()
        decrypted = decrypt_aes128_cbc(encrypted, key, iv)
        t2 = time.clock()
        encrypted2 = encrypt_pycrypto(plaintext, key, iv)
        t3 = time.clock()
        decrypted2 = decrypt_pycrypto(encrypted2, key, iv)
        t4 = time.clock()

        assert(plaintext == decrypted)
        assert(encrypted == encrypted2)
        assert(decrypted == decrypted2)

    print("%f mbyte"%(1e-6*n_bytes))
    print("encrypt_aes128_cbc: %f mbyte/sec"%(1e-6*n_bytes/(t1 - t0)))
    print("decrypt_aes128_cbc: %f mbyte/sec"%(1e-6*n_bytes/(t2 - t1)))
    print("encrypt_pycrypto: %f mbyte/sec"%(1e-6*n_bytes/(t3 - t2)))
    print("decrypt_pycrypto: %f mbyte/sec"%(1e-6*n_bytes/(t4 - t3)))
    print("")
