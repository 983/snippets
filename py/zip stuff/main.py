import struct
import zipfile
import zlib
import sys

"""
print(list(b"test"))
content = b"contentcontent\n"
print(len(content))
print(list(content))
"""

if 0:
    with zipfile.ZipFile("py_test.zip", "w") as f:
        f.write("asdf.txt")

with open("nested.zip", "rb") as f:
    data = f.read()
    """
    comment_length, = struct.unpack("<H", data[-2:])
    comment_offset = 2 + comment_length

    #end_header = data[-20-comment_offset: -comment_offset]
    offset = 2 + comment_length
    directory_offset, = struct.unpack("<I", data[-offset-4:-offset])
    print("directory_offset:", directory_offset)
    
    directory_size = 46
    i = directory_offset
    while True:
        directory = data[i:i+directory_size]; i += directory_size
        if not directory.startswith(b"PK\1\2"): break

        unpacked = struct.unpack("<IIIHHH", directory[16:34])
        crc, compressed_size, uncompressed_size, name_length, extra_length, comment_length = unpacked

        offset, = struct.unpack("<I", directory[-4:])

        print(offset, compressed_size, uncompressed_size)
        local_file_header_size = 30
        local_file_header = data[offset:offset + local_file_header_size]
        crc2, compressed_size2, uncompressed_size2, name_length2, extra_length2, = struct.unpack("<IIIHH", local_file_header[14:])
        offset += local_file_header_size
        
        filename = data[offset:offset+name_length2]; offset += name_length2
        print("filename:", filename)
        extra = data[offset:offset+extra_length2]; offset += extra_length2
        compressed = data[offset:offset+compressed_size2]; offset += compressed_size2
        print(data[offset:offset+4])
        
        print(crc, crc2, zlib.crc32(compressed))
        
        if len(compressed) > 0:
            decompressed = zlib.decompress(compressed)
            print(decompressed)
        
        
        name = data[i:i+name_length]; i += name_length
        i += extra_length
        i += comment_length


    """
    i = 0
    file_header_size = 30

    while True:
        file_header = data[i:i+file_header_size]; i += file_header_size
        if not file_header.startswith(b"PK\3\4"): break

        unpacked = struct.unpack("<IIIHH", file_header[14:])
        crc, compressed_size, uncompressed_size, name_length, extra_length = unpacked

        filename = data[i:i+name_length]; i += name_length
        print("name:", filename)
        extra = data[i:i+extra_length]; i += extra_length
        compressed = data[i:i+compressed_size]; i += compressed_size

        if compressed_size > 0:
            obj = zlib.decompressobj(wbits=-zlib.MAX_WBITS)
            decompressed = obj.decompress(compressed)
            assert(crc == zlib.crc32(decompressed))

        print("")
    

sys.exit(0)

if 0:
    with open("format2.txt", "w") as f:
        for row in data.split(b"PK"):
            row = " ".join(["%2x"%x for x in row])
            f.write(row + "\n")

files = [
    [b"asdf.txt", b""],
    [b"nested/", b""],
    [b"nested/asdfasdfasdf.txt", b""],
    [b"nested/testtesttest.txt", b"contentcontent\n"],
    [b"qwer.txt", b""],
]

local = []
central = []

offset = 0
central_size = 0
for path, data in files:
    version = 10
    attributes = 0x20
    if path.endswith(b"/"):
        attributes = 0x10
        version = 14
    
    compression = 0
    crc = zlib.crc32(data)
    
    # local headers
    local_header = b"PK\3\4"
    local_header += struct.pack(
        "<HHHHHIIII",
        version,
        0,
        compression,
        0,
        0,
        crc,
        len(data),
        len(data),
        len(path))
    local_header += path
    local_header += data

    local.append(local_header)
    
    #print(" ".join(["%2x"%x for x in header]))

    extra = bytes([0]*36)
    
    central_header = b"PK\1\2"
    central_header += struct.pack(
        "<HHHHHHIIIHHHHHII",
        0x3f,
        version,
        0,
        compression,
        0,
        0,
        crc,
        len(data),
        len(data),
        len(path),
        len(extra),
        0,
        0,
        0,
        attributes,
        offset)
    central_header += path
    central_header += extra

    central_size += len(central_header)
    offset += len(local_header)

    central.append(central_header)
    #print(" ".join(["%2x"%x for x in row]))

end = b"PK\5\6"
end += struct.pack(
    "<HHHHIIH",
    0,
    0,
    len(files),
    len(files),
    central_size,
    offset,
    0)
print(" ".join(["%2x"%x for x in end]))

with open("result.zip", "wb") as f:
    for data in local:
        f.write(data)
    for data in central:
        f.write(data)
    f.write(end)
