from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
import binascii
import getpass
import sys

def encrypt(plaintext, key):
    nonce = get_random_bytes(12)
    
    cipher = AES.new(
        key=key,
        mode=AES.MODE_GCM,
        nonce=nonce)
    
    encrypted, tag = cipher.encrypt_and_digest(plaintext)
    
    return nonce + encrypted + tag

def decrypt(encrypted, key):
    nonce = encrypted[:12]
    tag = encrypted[-16:]
    encrypted = encrypted[12:-16]
    
    cipher = AES.new(
        key=key,
        mode=AES.MODE_GCM,
        nonce=nonce)
    
    decrypted = cipher.decrypt_and_verify(encrypted, tag)
    
    return decrypted

def test():
    key = get_random_bytes(32)
    
    plaintext = bytes([1, 2, 3, 4, 5, 6, 7])

    encrypted = encrypt(plaintext, key)
    decrypted = decrypt(encrypted, key)
    
    assert(plaintext == decrypted)
    
    hex_key = "bcb1918edd91d816db0e14f3497d8868969f988659a3de120fb49c49b76a1a3a"
    
    key = binascii.unhexlify(hex_key)
    
    encrypted = bytes([
        130, 135, 224, 150, 68, 174, 156, 117, 239, 212, 218, 143, 185, 159, 75, 251, 84, 151, 73, 57, 177, 137, 46, 45, 189, 31, 174, 213, 205, 154, 0, 130, 231, 66, 111
    ])
    
    decrypted = decrypt(encrypted, key)
    
    assert(plaintext == decrypted)

def main():
    if len(sys.argv) != 4 or sys.argv[1][0] not in "ed":
        print("Usage: python crypt.py [e[ncrypt]/d[ecrypt]] <src path> <dst path>")
        return
    
    mode, src_path, dst_path = sys.argv[1:]
    
    if mode.startswith("e"):
        key = get_random_bytes(32)
        
        hex_key = binascii.hexlify(key).decode("ascii")
        
        print("password:")
        print(hex_key)
            
        with open(src_path, "rb") as f:
            plaintext = f.read()

        encrypted = encrypt(plaintext, key)

        with open(dst_path, "wb") as f:
            f.write(encrypted)

    if mode.startswith("d"):
        hex_key = getpass.getpass("Enter password:")
        
        key = binascii.unhexlify(hex_key)
    
        with open(src_path, "rb") as f:
            encrypted = f.read()

        decrypted = decrypt(encrypted, key)

        with open(dst_path, "wb") as f:
            f.write(decrypted)


if __name__ == "__main__":
    test()
    main()
