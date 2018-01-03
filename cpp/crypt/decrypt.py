import binascii
import hashlib
from Crypto.Cipher import AES
import sys

def load_bytes(path):
    with open(path, "rb") as f:
        return f.read()

encrypted = load_bytes("encrypted")
hash = encrypted[-32:]
encrypted = encrypted[:-32]
hash2 = hashlib.sha256(encrypted).digest()

assert(hash == hash2)

key = b"\0"*16
nonce = b"\0"*8

cipher = AES.new(key, AES.MODE_CTR, nonce=nonce)
decrypted = cipher.decrypt(encrypted)

plaintext = load_bytes("garbage/eax.pdf")

assert(decrypted == plaintext)

"""
# something like "password_9c4d8404d4d4397f389e55f5dd3875b5.txt"
path = sys.argv[1]

with open(path, "r") as f:
    d = {}
    for line in f.readlines():
        key, value = line.split()
        d[key] = value

name = d["name"]
algorithm = d["algorithm"]
assert(algorithm == "aes128-ctr-hmac")
key = binascii.unhexlify(d["key"])
hmac = binascii.unhexlify(d["hmac"])
nonce = binascii.unhexlify(d["nonce"])

encrypted = load_bytes(name)

assert(hmac == hashlib.sha256(encrypted).digest())

print("sha256 hash matches!")

cipher = AES.new(key, AES.MODE_CTR, nonce=nonce)
decrypted = cipher.decrypt(encrypted)

plaintext = load_bytes("garbage/eax.pdf")

assert(len(plaintext) == len(decrypted))
assert(plaintext == decrypted)

print("decrypted successfully!")
"""
