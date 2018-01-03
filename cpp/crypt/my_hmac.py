import binascii
import hashlib
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
import sys
import binascii
import hmac
import hashlib

def load_bytes(path):
    with open(path, "rb") as f:
        return f.read()

# common problems:
# padding oracle
# chosen plaintext for aes-ctr (content can be modified without key if plaintext is known)

def my_hmac(key, message):
    # 64
    blocksize = hashlib.sha256().block_size
    
    if len(key) > blocksize:
        key = hashlib.sha256(key).digest()

    key += b"\0"*(blocksize - len(key))

    inner = hashlib.sha256(bytes(x ^ 0x36 for x in key))
    outer = hashlib.sha256(bytes(x ^ 0x5c for x in key))

    inner.update(message)

    outer.update(inner.digest())
    
    return outer

for message_length in range(100):
    message = get_random_bytes(message_length)
    for key_length in range(100):
        key = get_random_bytes(key_length)
    
        a = hmac.new(key=key, msg=message, digestmod='sha256').hexdigest()
        b = my_hmac(key=key, message=message).hexdigest()
        
        assert(a == b)

print("checks out")

key = bytes([4, 5, 6])
message = bytes([1, 3, 3, 7])

result = my_hmac(key, message).digest()
print(len(result))
print(list(result))
print(result)

"""
key = b"\0"*16
nonce = b"\0"*12

cipher = AES.new(key, AES.MODE_CCM, nonce=nonce)

plaintext = b"Hello, World!123"

encrypted, tag = cipher.encrypt_and_digest(plaintext)

print(len(encrypted))
print(len(tag))
print(list(tag))
print(list(encrypted))



for i in range(0, len(plaintext), 16):
    block = plaintext[i:i+16]

print(list(AES.new(key, AES.MODE_CTR, nonce=nonce).encrypt(plaintext)))
"""

rows = """
K:          40414243 44454647 48494a4b 4c4d4e4f
N:          10111213 141516
A:          00010203 04050607
P:          20212223 
B:          4f101112 13141516 00000000 00000004 00080001 02030405 06070000 00000000 20212223 00000000 00000000 00000000
T:          6084341b
Ctr0:       07101112 13141516 00000000 00000000
S0:         2d281146 10676c26 32bad748 559a679a
Ctr1:       07101112 13141516 00000000 00000001
S1:         51432378 e474b339 71318484 103cddfb
C:          7162015b 4dac255d 
"""

"""
K is the key,
N is the nonce,
A is the associated data,
P is the plaintext,
B is the input to the MAC,
T is the authentication tag,
Ctr0 is the first counter value,
Ctr1 is the second,
C is the ciphertext and
S0 is the encryption of Ctr0
S1 is the encryption of Ctr1
"""
"""
for row in rows.strip().split("\n"):
    key, value = row.split(":")
    key = key.strip()
    value = value.replace(" ", "")
    value = binascii.unhexlify(value)
    print(key, value)
"""

"""
hdr = b'To your eyes only'
plaintext = b'Attack at dawn'
key = b'Sixteen byte key'
#nonce = get_random_bytes(11)
nonce = b"\0"*11
cipher = AES.new(key, AES.MODE_CCM, nonce)
#cipher.update(hdr)
msg = nonce, hdr, cipher.encrypt(plaintext), cipher.digest()

nonce, hdr, ciphertext, mac = msg
key = b'Sixteen byte key'
cipher = AES.new(key, AES.MODE_CCM, nonce)
#cipher.update(hdr)
plaintext = cipher.decrypt(ciphertext)
cipher.verify(mac)

print(ciphertext)

print(AES.new(key, AES.MODE_CTR, nonce=nonce).encrypt(plaintext))
"""
