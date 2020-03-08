import tarfile
import struct

def read_tar(path, encoding="utf-8"):
    with open(path, "rb") as f:
        # http://www.onicos.com/staff/iz/formats/tar.html
        header_format = [
            ("filename", 100),
            ("filemode", 8),
            ("userid", 8),
            ("groupid", 8),
            ("filesize", 12),
            ("modifytime", 12),
            ("headerchecksum", 8),
            ("linkflag", 1),
            ("linkname", 100),
            ("magic", 8),
            ("username", 32),
            ("groupname", 32),
            ("majordeviceid", 8),
            ("minordeviceid", 8),
            ("padding", 167),
        ]

        while True:
            header_pos = 0
            header_data = f.read(512)
            
            if len(header_data) == 0 or header_data == b"\0"*512:
                break

            header = {}

            for name, length in header_format:
                value = header_data[header_pos:header_pos+length]
                header_pos += length

                header[name] = value

            assert(header["magic"] == b'ustar  \x00')

            filesize = int(header["filesize"].rstrip(b"\0"), 8)
            
            data = f.read(filesize)

            assert(len(data) == filesize)

            padding = (512 - filesize % 512) % 512
            
            f.seek(padding, 1)

            filename = header["filename"].rstrip(b"\0").decode(encoding)

            yield data, filename

if __name__ == "__main__":

    for path in [
        "thumbnails128x128.tar",
        "celeba-dataset.tar",
    ]:
        for data, filename in read_tar(path):
            print(filename)
