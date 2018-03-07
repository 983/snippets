import struct
import zlib

files = [
    [b"asdf.txt", b""],
    [b"nested/", b""],
    [b"nested/asdfasdfasdf.txt", b"asdfasdfasdf"],
    [b"nested/testtesttest.txt", b"testtesttest\n"*10],
    [b"qwer.txt", b""],
]

local = []
central = []

local_size = 0
central_size = 0
for path, data in files:
    is_directory = path.endswith(b"/")
    
    version = 10
    attributes = 0x20
    if is_directory:
        attributes = 0x10
        version = 14

    crc = zlib.crc32(data)
    
    compression = 0
    compressed_data = data
    
    local_header = b"PK\3\4"
    local_header += struct.pack(
        "<HHHHHIIII",
        version,
        0,
        compression,
        0,
        0,
        crc,
        len(compressed_data),
        len(data),
        len(path))

    local.append(local_header)
    local.append(path)
    local.append(compressed_data)

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
        len(compressed_data),
        len(data),
        len(path),
        0,
        0,
        0,
        0,
        attributes,
        local_size)

    print(list(central_header))
    
    central_size += len(central_header) + len(path)
    local_size += len(local_header) + len(path) + len(compressed_data)

    central.append(central_header)
    central.append(path)

end = b"PK\5\6"
end += struct.pack(
    "<HHHHIIH",
    0,
    0,
    len(files),
    len(files),
    central_size,
    local_size,
    0)

combined = b"".join(local) + b"".join(central) + end

with open("result.zip", "wb") as f:
    f.write(combined)
"""
with open("result2.zip", "rb") as f:
    data = f.read()
    assert(data == combined)
"""
