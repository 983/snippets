from Crypto.Cipher import AES
from Crypto import Random

BLOCK_SIZE = 16

def pad(s):
    n_pad = BLOCK_SIZE - len(s) % BLOCK_SIZE
    padding = bytes([n_pad])*n_pad
    return s + padding

def unpad(s):
    n_pad = s[-1]
    padding = bytes([n_pad])*n_pad

    if n_pad > BLOCK_SIZE or s[-n_pad:] != padding:
        raise Exception()
    
    return s[:-n_pad]

# TODO
# handle 16 bytes padding
# handle padding which accidentally looks like e.g. \x02 \x02 but is actually something else
# can get first block without knowing iv? probably not

key = bytes([96, 74, 10, 136, 56, 213, 240, 94, 26, 233, 34, 92, 239, 167, 185, 201])
iv = bytes([109, 43, 110, 73, 5, 72, 12, 162, 134, 149, 230, 17, 97, 244, 31, 64])
plaintext = Random.new().read(BLOCK_SIZE*2 - 7)

plaintext = pad(plaintext)

print("padded plaintext of length %d:"%len(plaintext))
print(list(plaintext))

cipher = AES.new(key, AES.MODE_CBC, iv)
encrypted = cipher.encrypt(plaintext)

block1 = encrypted[-2*BLOCK_SIZE:-1*BLOCK_SIZE]
block2 = encrypted[-1*BLOCK_SIZE:]

print("")
print("decrypted:")

for guess in range(256):
    cipher = AES.new(key, AES.MODE_CBC, iv)

    a = [0]*16
    a[-1] = guess
    
    s = cipher.decrypt(bytes(a) + block2)
    
    n_pad = s[-1]
    padding = bytes([n_pad])*n_pad

    if n_pad <= BLOCK_SIZE and s[-n_pad:] == padding:
        plain = s[-1] ^ block1[-1] ^ guess
        print(plain)
        break

n_pad = plain

print("%d padding symbols"%n_pad)

for k in range(n_pad + 1, BLOCK_SIZE + 1):

    for guess in range(256):
        cipher = AES.new(key, AES.MODE_CBC, iv)

        a = [0]*16
        a[-k] = guess
        
        s = cipher.decrypt(bytes(a) + block2)

        if s[-k] == k:
            plain = k ^ a[-k] ^ block1[-k]
            print(plain)
        
