import struct
import zlib

files = [
    #[b"nested/", b""],
    [b"nested/asdf3.txt", b"asdf\n"*3],
    [b"nested/test1024.txt", b"test\n"*1024],
]

use_compression = True

local = []
central = []

local_size = 0
central_size = 0
for path, data in files:
    is_directory = path.endswith(b"/")
    
    version = 0x14
    attributes = 0x10 if is_directory else 0x20
    
    crc = zlib.crc32(data)
    
    if use_compression and not is_directory:
        compression = 8
        obj = zlib.compressobj(wbits=-zlib.MAX_WBITS)
        compressed_data = obj.compress(data)
        compressed_data += obj.flush()
    else:
        compression = 0
        compressed_data = data
    
    print("compressed_data:", len(compressed_data))
    
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

    #print(" ".join("%2x"%x for x in local_header + path + compressed_data))

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

    #print(" ".join("%2x"%x for x in central_header + path))
    
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

#print(" ".join("%2x"%x for x in end))

combined = b"".join(local) + b"".join(central) + end

for row in combined.split(b"PK"):
    if len(row) == 0: continue
    row = b"PK" + row
    print(" ".join("%2x"%x for x in row))

with open("result.zip", "wb") as f:
    f.write(combined)
