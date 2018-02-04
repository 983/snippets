#!/usr/bin/env python3
from PIL import Image
import os
import sys
import hashlib
from getpass import getpass
from struct import pack, unpack
from Crypto.Cipher import AES
from Crypto.Util import Counter
from Crypto.Random import get_random_bytes

def split(values, n):
    result = []
    for i in range(0, len(values), n):
        result.append(values[i:i+n])
    return result

# salt, mac, nonce, len(block)
ENCRYPT_HEADER_SIZE = 32 + 16 + 11 + 8

def encrypt(plaintext, password, pad_size):
    # Encrypts plaintext and pads it to pad_size.
    # The minimal encrypted size is len(plaintext) + ENCRYPT_HEADER_SIZE.
    
    salt = get_random_bytes(32)
    key = hashlib.pbkdf2_hmac('sha256', password, salt, 10*1000)

    padding = pad_size - len(plaintext) - ENCRYPT_HEADER_SIZE
    assert(padding >= 0)
    # pad with zeros and length in bytes
    to_be_encrypted = plaintext + b'\0'*padding + pack('<Q', len(plaintext))

    nonce = get_random_bytes(11)
    cipher = AES.new(key, AES.MODE_CCM, nonce)
    encrypted, mac = cipher.encrypt_and_digest(to_be_encrypted)

    return salt + mac + nonce + encrypted

def decrypt(encrypted, password):
    salt = encrypted[:32]
    mac = encrypted[32:48]
    nonce = encrypted[48:59]
    encrypted = encrypted[59:]
    
    key = hashlib.pbkdf2_hmac('sha256', password, salt, 10*1000)

    cipher = AES.new(key, AES.MODE_CCM, nonce)
    
    decrypted_and_paded = cipher.decrypt_and_verify(encrypted, mac)

    size = unpack('<Q', decrypted_and_paded[-8:])[0]

    decrypted = decrypted_and_paded[:size]

    return decrypted

def encrypt_file(path, directory, password, width, height):
    pad_size = width*height*3

    try:
        os.mkdir(directory)
    except FileExistsError:
        pass

    with open(path, 'rb') as f:
        data = f.read()

    filename = os.path.basename(path)

    filename = filename.encode('utf-8')
    password = password.encode('utf-8')

    # three uint64 + filename
    block_header_size = 8*3 + len(filename)

    # split data into blocks, but leave space for headers
    blocks = split(data, pad_size - ENCRYPT_HEADER_SIZE - block_header_size)

    # prepend block header
    blocks = [pack('<QQQ', i, len(blocks), len(filename)) + filename + block
              for i, block in enumerate(blocks)]

    blocks = [encrypt(block, password, pad_size) for block in blocks]

    for i, block in enumerate(blocks):
        print('Processing image %d of %d'%(i, len(blocks)))
        image = Image.frombytes('RGB', (width, height), block)
        image.save(os.path.join(directory, '%d.png'%i))
    print('')
    print('Images written to directory: "%s"'%directory)

def decrypt_images(directory, password, output_filename=None):
    password = password.encode('utf-8')
    
    blocks = {}
    for filename in os.listdir(directory):
        path = os.path.join(directory, filename)
        print('Processing file: "%s"'%path)
        try:
            image = Image.open(path)
            encrypted = image.tobytes()
        except Exception as e:
            print(e)
            continue

        try:
            decrypted = decrypt(encrypted, password)
        except ValueError:
            print('ERROR - Wrong password')
            return
            
        block_index, block_count, filename_size = unpack('<QQQ', decrypted[:3*8])
        filename = decrypted[3*8:3*8+filename_size]
        filename = filename.decode('utf-8')

        if output_filename is None:
            output_filename = filename

        if os.path.isfile(output_filename):
            print('ERROR - File "%s" exists'%output_filename)
            return

        block = decrypted[3*8+filename_size:]

        blocks[block_index] = block

    print('')

    missing = set(range(block_count)) ^ set(blocks.keys())

    if missing:
        print('ERROR - Missing images:')
        for i in missing:
            print(i)
        return

    with open(output_filename, 'wb') as f:
        for i in range(block_count):
            f.write(blocks[i])
    
    print('File written to: "%s"'%output_filename)

# tests
if 1:
    # test basic encryption and decryption
    plaintext = b'Hello, World!'
    password = b'hunter2'
    encrypted = encrypt(plaintext, password, pad_size=1024)
    decrypted = decrypt(encrypted, password)
    assert(plaintext == decrypted)

    # message authentication should fail if a bit is flipped
    encrypted = list(encrypted)
    # flip a bit
    encrypted[-1] ^= 1
    encrypted = bytes(encrypted)
    try:
        decrypted = decrypt(encrypted, password)
        raise Exception('decrypt on modified data should have failed')
    except ValueError:
        pass

    # encrypt and decrypt a few megabytes of data
    plaintext = b'0123456789'*(1024*1024)
    password = get_random_bytes(128)
    encrypted = encrypt(plaintext, password, pad_size=len(plaintext) + ENCRYPT_HEADER_SIZE)
    decrypted = decrypt(encrypted, password)
    assert(plaintext == decrypted)

def print_usage():
    print('Usage:')
    print('%s --encrypt input_file output_directory [--size width height]'%sys.argv[0])
    print('%s --decrypt input_directory [output_file]'%sys.argv[0])

if __name__ == '__main__':
    if len(sys.argv) in [4, 7] and sys.argv[1] in ['-e',  '--encrypt']:
        path = sys.argv[2]
        directory = sys.argv[3]

        password = getpass()

        width = 640
        height = 480

        if len(sys.argv) == 7 and sys.argv[4] in ['-s', '--size']:
            width = int(sys.argv[5])
            height = int(sys.argv[6])
        
        encrypt_file(path, directory, password, width, height)
        
    elif len(sys.argv) in [3, 4] and sys.argv[1] in ['-d', '--decrypt']:
        directory = sys.argv[2]

        if not os.path.isdir(directory):
            print('ERROR - "%s" is not a directory'%directory)
            sys.exit(0)

        output_filename = None
        if len(sys.argv) == 4:
            output_filename = sys.argv[3]

        password = getpass()

        decrypt_images(directory, password, output_filename)
        
    else:
        print_usage()
