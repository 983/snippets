import zlib, time
import cupy as cp
import numpy as np

check_crc32 = cp.RawKernel(
    r"""
typedef unsigned int u32;

#define STEP(crc) for (int j = 0; j < 8; j++){ crc = (crc >> 1) ^ ((crc & 1) ? polynomial : 0); }

extern "C" __global__
void check_crc32(unsigned char *data, int n){
    const u32 polynomial = 0xedb88320;

    unsigned char c0 = data[0] + threadIdx.x;
    unsigned char c1 = data[1] + threadIdx.y;
    unsigned char c2 = data[2] + blockIdx.x;
    unsigned char c3 = data[3] + blockIdx.y;

    u32 initial_crc = 0;

    initial_crc ^= 0xffffffff;

    initial_crc ^= c0; STEP(initial_crc)
    initial_crc ^= c1; STEP(initial_crc)
    initial_crc ^= c2; STEP(initial_crc)
    initial_crc ^= c3; STEP(initial_crc)

    u32 target_crc = 0;

    for (u32 c4 = '0'; c4 < '9'; c4++){
        for (u32 c5 = '0'; c5 <= '9'; c5++){
            for (u32 c6 = '0'; c6 < '9'; c6++){
                for (u32 c7 = '0'; c7 <= '9'; c7++){
                    for (u32 c8 = '0'; c8 < '9'; c8++){
                        for (u32 c9 = '0'; c9 <= '9'; c9++){
                            u32 crc = initial_crc;

                            crc ^= c4; STEP(crc)
                            crc ^= c5; STEP(crc)
                            crc ^= c6; STEP(crc)
                            crc ^= c7; STEP(crc)
                            crc ^= c8; STEP(crc)
                            crc ^= c9; STEP(crc)

                            for (int i = 10; i < n; i++){
                                crc ^= data[i];

                                STEP(crc)
                            }

                            crc ^= 0xffffffff;

                            if (crc == target_crc){
                                printf("Prefix to get target crc %u: %c%c%c%c%c%c%c%c%c%c\n", crc, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9);
                            }
                        }
                    }
                }
            }
        }
    }
}
""",
    "check_crc32",
)

suffix = " Suffix to be appended to 10 digits."

print(zlib.crc32(("7472386182" + suffix).encode("utf-8")))

if 1:
    t = time.perf_counter()
    data = cp.asarray(np.uint8(list(("0000000000" + suffix).encode("utf-8"))))
    args = (data, len(data))
    block_size = (10, 10)
    grid_size = (10, 10)
    check_crc32(grid_size, block_size, args)
    dt = time.perf_counter() - t
    print(dt, "seconds")
    print(10**10 * 1e-9 / dt, "billion CRCs per second")
