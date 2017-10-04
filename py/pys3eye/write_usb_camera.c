void ov534_set_led(libusb_device_handle *handle, int status){
    
    uint8_t data = ov534_reg_read(handle, 0x21);
    data |= 0x80;
    ov534_reg_write(handle, 0x21, data);

    data = ov534_reg_read(handle, 0x23);
    
    if (status){
        data |= 0x80;
    }else{
        data &= ~0x80;
    }

    ov534_reg_write(handle, 0x23, data);
    
    if (!status){
        data = ov534_reg_read(handle, 0x21);
        data &= ~0x80;
        ov534_reg_write(handle, 0x21, data);
    }
}

uint16_t ov534_set_frame_rate(
    libusb_device_handle *handle,
    uint16_t frame_rate
){
    // TODO different frame rates
    
    sccb_reg_write(handle, 0x11, 0x00);
    sccb_reg_write(handle, 0x0d, 0x41);
    ov534_reg_write(handle, 0xe5, 0x04);
    
#if 0
    WHERE
    
     int i;
     struct rate_s {
             uint16_t fps;
             uint8_t r11;
             uint8_t r0d;
             uint8_t re5;
     };
     const struct rate_s *r;
     static const struct rate_s rate_0[] = { /* 640x480 */
             {83, 0x01, 0xc1, 0x02}, /* 83 FPS: video is partly corrupt */
             {75, 0x01, 0x81, 0x02}, /* 75 FPS or below: video is valid */
             {60, 0x00, 0x41, 0x04},
             {50, 0x01, 0x41, 0x02},
             {40, 0x02, 0xc1, 0x04},
             {30, 0x04, 0x81, 0x02},
             {25, 0x00, 0x01, 0x02},
             {20, 0x04, 0x41, 0x02},
             {15, 0x09, 0x81, 0x02},
             {10, 0x09, 0x41, 0x02},
             {8, 0x02, 0x01, 0x02},
             {5, 0x04, 0x01, 0x02},
             {3, 0x06, 0x01, 0x02},
             {2, 0x09, 0x01, 0x02},
     };
     static const struct rate_s rate_1[] = { /* 320x240 */
             {290, 0x00, 0xc1, 0x04},
             {205, 0x01, 0xc1, 0x02}, /* 205 FPS or above: video is partly corrupt */
             {187, 0x01, 0x81, 0x02}, /* 187 FPS or below: video is valid */
             {150, 0x00, 0x41, 0x04},
             {137, 0x02, 0xc1, 0x02},
             {125, 0x01, 0x41, 0x02},
             {100, 0x02, 0xc1, 0x04},
             {90, 0x03, 0x81, 0x02},
             {75, 0x04, 0x81, 0x02},
             {60, 0x04, 0xc1, 0x04},
             {50, 0x04, 0x41, 0x02},
             {40, 0x06, 0x81, 0x03},
             {37, 0x00, 0x01, 0x04},
             {30, 0x04, 0x41, 0x04},
             {17, 0x18, 0xc1, 0x02},
             {15, 0x18, 0x81, 0x02},
             {12, 0x02, 0x01, 0x04},
             {10, 0x18, 0x41, 0x02},
             {7, 0x04, 0x01, 0x04},
             {5, 0x06, 0x01, 0x04},
             {3, 0x09, 0x01, 0x04},
             {2, 0x18, 0x01, 0x02},
     };

     if (frame_width == 640) {
             r = rate_0;
             i = ARRAY_SIZE(rate_0);
     } else {
             r = rate_1;
             i = ARRAY_SIZE(rate_1);
     }
     while (--i > 0) {
             if (frame_rate >= r->fps)
                     break;
             r++;
     }
 
     if (!dry_run) {
        printf("set_fps: 0x%02x 0x%02x 0x%02x\n", r->r11, r->r0d, r->re5);
         
        sccb_reg_write(0x11, r->r11);
        sccb_reg_write(0x0d, r->r0d);
        ov534_reg_write(0xe5, r->re5);
    }

     debug("frame_rate: %d\n", r->fps);
     return r->fps;
#endif
}

void setAutoWhiteBalance(libusb_device_handle *handle, int val){
    if (val){
        sccb_reg_write(handle, 0x63, 0xe0); //AWB ON
    }else{
        sccb_reg_write(handle, 0x63, 0xAA); //AWB OFF
    }
}

void setGain(libusb_device_handle *handle, uint8_t val){
    
    switch(val & 0x30){
    case 0x00:
        val &=0x0F;
        break;
    case 0x10:
        val &=0x0F;
        val |=0x30;
        break;
    case 0x20:
        val &=0x0F;
        val |=0x70;
        break;
    case 0x30:
        val &=0x0F;
        val |=0xF0;
        break;
    }
    
    sccb_reg_write(handle, 0x00, val);
}

void setExposure(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x08, val>>7);
    sccb_reg_write(handle, 0x10, val<<1);
}

void setSharpness(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x91, val); //vga noise
    sccb_reg_write(handle, 0x8E, val); //qvga noise
}

void setContrast(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x9C, val);
}

void setBrightness(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x9B, val);
}

void setHue(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x01, val);
}

void setRedBalance(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x43, val);
}

void setBlueBalance(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x42, val);
}

void setGreenBalance(libusb_device_handle *handle, uint8_t val){
    sccb_reg_write(handle, 0x44, val);
}

void setFlip(libusb_device_handle *handle, int horizontal, int vertical){
    uint8_t val = sccb_reg_read(handle, 0x0c);
    val &= ~0xc0;
    if (!horizontal) val |= 0x40;
    if (!vertical) val |= 0x80;
    sccb_reg_write(handle, 0x0c, val);
}

void setAutogain(libusb_device_handle *handle, int val, uint8_t gain, uint8_t exposure){
    
    if (val){
        sccb_reg_write(handle, 0x13, 0xf7); //AGC,AEC,AWB ON
        sccb_reg_write(handle, 0x64, sccb_reg_read(handle, 0x64)|0x03);
    } else {
        sccb_reg_write(handle, 0x13, 0xf0); //AGC,AEC,AWB OFF
        sccb_reg_write(handle, 0x64, sccb_reg_read(handle, 0x64)&0xFC);

        setGain(handle, gain);
        setExposure(handle, exposure);
    }
}
