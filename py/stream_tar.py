import struct
import os

class Stream(object):
    def __init__(self, data):
        self.data = data
        self.pos = 0
    
    def remaining(self):
        return len(self.data) - self.pos
    
    def read(self, n):
        assert(self.remaining() >= n)
        
        data = self.data[self.pos:self.pos+n]
        
        self.pos += n
        
        return data

def stream_tar(f):
    linkflag_to_filetype = {
        b"\0": "file",
        b"0": "file",
        b"1": "link",
        b"2": "symlink",
        b"3": "character",
        b"4": "block",
        b"5": "directory",
        b"6": "fifo",
        b"7": "contiguous",
        b"x": "paxheader",
        
    }
    pos = 0
    while True:
        header = f.read(512)
        pos += 512
        
        # tar file is terminated by two zero headers
        if len(header.strip(b"\0")) == 0: break
        
        s = Stream(header)
            
        filename    = s.read(100)
        filemode    = s.read(8)
        userid      = s.read(8)
        group_id    = s.read(8)
        filesize    = s.read(12)
        modify_time = s.read(12)
        checksum    = s.read(8)
        linkflag    = s.read(1)
        linkname    = s.read(100)
        magic       = s.read(8)
        username    = s.read(32)
        groupname   = s.read(32)
        major_id    = s.read(8)
        minor_id    = s.read(8)
        prefix      = s.read(167)

        filesize = int(filesize, 8)
        checksum = int(checksum.strip(b"\0 "), 8)
        prefix = prefix.strip(b"\0")
        
        if len(prefix) > 0:
            filename = prefix + b"/" + filename
        
        filename = filename.strip(b"\0").decode("utf-8")
        
        assert(magic == b'ustar\x0000')
        assert(linkflag in linkflag_to_filetype)
        
        if linkflag == b"0":
            yield (filename, filesize)
        
        pos += filesize
        
        # round up pos to multiples of 512
        pos = (pos + 511) & ~511
        
        f.seek(pos, 0)

path = "/home/hugo/Desktop/todo/1.tar"

with open(path, "rb") as f:
    
    for filename, filesize in stream_tar(f):
        #data = f.read(filesize)
        print(filename)
