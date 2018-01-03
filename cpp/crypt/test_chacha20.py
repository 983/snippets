from Crypto.Random import get_random_bytes
from Crypto.Cipher import ChaCha20

plaintext = get_random_bytes(150)
key = get_random_bytes(32)
nonce = get_random_bytes(8)

cipher = ChaCha20.new(key=key, nonce=nonce)
msg = cipher.nonce + cipher.encrypt(plaintext)

nonce = msg[:8]
encrypted = msg[8:]

cipher = ChaCha20.new(key=key, nonce=nonce)
plaintext = cipher.decrypt(encrypted)

print(list(plaintext))
print("")
print(list(key))
print("")
print(list(nonce))
print("")
print(list(encrypted))
print("")
