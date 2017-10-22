#define VENDOR_ID           0x1415
#define PRODUCT_ID          0x2000

#define TRANSFER_SIZE       65536
#define NUM_TRANSFERS       5

#define OV534_REG_ADDRESS   0xf1    /* sensor address */
#define OV534_REG_SUBADDR   0xf2
#define OV534_REG_WRITE     0xf3
#define OV534_REG_READ      0xf4
#define OV534_REG_OPERATION 0xf5
#define OV534_REG_STATUS    0xf6

#define OV534_OP_WRITE_3    0x37
#define OV534_OP_WRITE_2    0x33
#define OV534_OP_READ_2     0xf9

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_A) (sizeof(_A) / sizeof((_A)[0]))
#endif

static const uint8_t ov534_reg_initdata[][2] = {
    { 0xe7, 0x3a },

    { OV534_REG_ADDRESS, 0x42 }, /* select OV772x sensor */

    { 0x92, 0x01 },
    { 0x93, 0x18 },
    { 0x94, 0x10 },
    { 0x95, 0x10 },
    { 0xE2, 0x00 },
    { 0xE7, 0x3E },
    
    { 0x96, 0x00 },
    { 0x97, 0x20 },
    { 0x97, 0x20 },
    { 0x97, 0x20 },
    { 0x97, 0x0A },
    { 0x97, 0x3F },
    { 0x97, 0x4A },
    { 0x97, 0x20 },
    { 0x97, 0x15 },
    { 0x97, 0x0B },

    { 0x8E, 0x40 },
    { 0x1F, 0x81 },
    { 0xC0, 0x50 },
    { 0xC1, 0x3C },
    { 0xC2, 0x01 },
    { 0xC3, 0x01 },
    { 0x50, 0x89 },
    { 0x88, 0x08 },
    { 0x8D, 0x00 },
    { 0x8E, 0x00 },

    { 0x1C, 0x00 },     /* video data start (V_FMT) */

    { 0x1D, 0x00 },     /* RAW8 mode */
    { 0x1D, 0x02 },     /* payload size 0x0200 * 4 = 2048 bytes */
    { 0x1D, 0x00 },     /* payload size */

    { 0x1D, 0x01 },     /* frame size = 0x012C00 * 4 = 307200 bytes (640 * 480 @ 8bpp) */
    { 0x1D, 0x2C },     /* frame size */
    { 0x1D, 0x00 },     /* frame size */

    { 0x1C, 0x0A },     /* video data start (V_CNTL0) */
    { 0x1D, 0x08 },     /* turn on UVC header */
    { 0x1D, 0x0E },

    { 0x34, 0x05 },
    { 0xE3, 0x04 },
    { 0x89, 0x00 },
    { 0x76, 0x00 },
    { 0xE7, 0x2E },
    { 0x31, 0xF9 },
    { 0x25, 0x42 },
    { 0x21, 0xF0 },
    { 0xE5, 0x04 }  
};

static const uint8_t ov772x_reg_initdata[][2] = {

    { 0x12, 0x80 },     /* reset */
    { 0x3D, 0x00 },

    { 0x12, 0x01 },     /* Processed Bayer RAW (8bit) */

    { 0x11, 0x01 },
    { 0x14, 0x40 },
    { 0x15, 0x00 },
    { 0x63, 0xAA },     // AWB  
    { 0x64, 0x87 },
    { 0x66, 0x00 },
    { 0x67, 0x02 },
    { 0x17, 0x26 },
    { 0x18, 0xA0 },
    { 0x19, 0x07 },
    { 0x1A, 0xF0 },
    { 0x29, 0xA0 },
    { 0x2A, 0x00 },
    { 0x2C, 0xF0 },
    { 0x20, 0x10 },
    { 0x4E, 0x0F },
    { 0x3E, 0xF3 },
    { 0x0D, 0x41 },
    { 0x32, 0x00 },
    { 0x13, 0xF0 },     // COM8  - jfrancois 0xf0   orig x0f7
    { 0x22, 0x7F },
    { 0x23, 0x03 },
    { 0x24, 0x40 },
    { 0x25, 0x30 },
    { 0x26, 0xA1 },
    { 0x2A, 0x00 },
    { 0x2B, 0x00 },
    { 0x13, 0xF7 },
    { 0x0C, 0xC0 },

    { 0x11, 0x00 },
    { 0x0D, 0x41 },

    { 0x8E, 0x00 },     // De-noise threshold - jfrancois 0x00 - orig 0x04
};

static const uint8_t bridge_start_vga[][2] = {
    {0x1c, 0x00},
    {0x1d, 0x00},
    {0x1d, 0x02},
    {0x1d, 0x00},
    {0x1d, 0x01},   /* frame size = 0x012C00 * 4 = 307200 bytes (640 * 480 @ 8bpp) */
    {0x1d, 0x2C},   /* frame size */
    {0x1d, 0x00},   /* frame size */
    {0xc0, 0x50},
    {0xc1, 0x3c},
};
static const uint8_t sensor_start_vga[][2] = {
    {0x12, 0x01},
    {0x17, 0x26},
    {0x18, 0xa0},
    {0x19, 0x07},
    {0x1a, 0xf0},
    {0x29, 0xa0},
    {0x2c, 0xf0},
    {0x65, 0x20},
};
static const uint8_t bridge_start_qvga[][2] = {
    {0x1c, 0x00},
    {0x1d, 0x00},
    {0x1d, 0x02},
    {0x1d, 0x00},   
    {0x1d, 0x00},   /* frame size = 0x004B00 * 4 = 76800 bytes (320 * 240 @ 8bpp) */
    {0x1d, 0x4b},   /* frame size */
    {0x1d, 0x00},   /* frame size */
    {0xc0, 0x28},
    {0xc1, 0x1e},
};
static const uint8_t sensor_start_qvga[][2] = {
    {0x12, 0x41},
    {0x17, 0x3f},
    {0x18, 0x50},
    {0x19, 0x03},
    {0x1a, 0x78},
    {0x29, 0x50},
    {0x2c, 0x78},
    {0x65, 0x2f},
};

/* Values for bmHeaderInfo (Video and Still Image Payload Headers, 2.4.3.3) */
#define UVC_STREAM_EOH  (1 << 7)
#define UVC_STREAM_ERR  (1 << 6)
#define UVC_STREAM_STI  (1 << 5)
#define UVC_STREAM_RES  (1 << 4)
#define UVC_STREAM_SCR  (1 << 3)
#define UVC_STREAM_PTS  (1 << 2)
#define UVC_STREAM_EOF  (1 << 1)
#define UVC_STREAM_FID  (1 << 0)