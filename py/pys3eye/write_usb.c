
void ov534_reg_write(
    libusb_device_handle *handle,
    uint16_t reg,
    uint8_t val
){
    uint8_t buf[64];
    buf[0] = val;

    int ret = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | 
        LIBUSB_REQUEST_TYPE_VENDOR |
        LIBUSB_RECIPIENT_DEVICE, 
        0x01, 0x00, reg, buf, 1, 500);
    
    assert(ret >= 0);
}

uint8_t ov534_reg_read(
    libusb_device_handle *handle,
    uint16_t reg
){
    uint8_t buf[64];
    
    int ret = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR | 
        LIBUSB_RECIPIENT_DEVICE,
        0x01, 0x00, reg, buf, 1, 500);

    assert(ret >= 0);

    return buf[0];
}

int sccb_check_status(libusb_device_handle *handle){
    for (int i = 0; i < 5; i++){
        uint8_t data = ov534_reg_read(handle, OV534_REG_STATUS);

        switch (data){
            case 0x00: return 1;
            case 0x04: return 0;
            case 0x03: break;
            default: printf("sccb status 0x%02x, attempt %d/5\n", data, i + 1);
        }
    }
    return 0;
}

void sccb_reg_write(
    libusb_device_handle *handle,
    uint8_t reg,
    uint8_t val
){
    ov534_reg_write(handle, OV534_REG_SUBADDR, reg);
    ov534_reg_write(handle, OV534_REG_WRITE, val);
    ov534_reg_write(handle, OV534_REG_OPERATION, OV534_OP_WRITE_3);

    if (!sccb_check_status(handle)){
        printf("sccb_reg_write failed\n");
    }
}

uint8_t sccb_reg_read(libusb_device_handle *handle, uint16_t reg){
    ov534_reg_write(handle, OV534_REG_SUBADDR, (uint8_t)reg);
    ov534_reg_write(handle, OV534_REG_OPERATION, OV534_OP_WRITE_2);
    
    // TODO handle errors properly
    if (!sccb_check_status(handle)) {
        printf("sccb_reg_read failed 1\n");
    }

    ov534_reg_write(handle, OV534_REG_OPERATION, OV534_OP_READ_2);
    
    if (!sccb_check_status(handle)) {
        printf( "sccb_reg_read failed 2\n");
    }

    return ov534_reg_read(handle, OV534_REG_READ);
}

void reg_w_array(
    libusb_device_handle *handle,
    const uint8_t (*data)[2],
    int len
){
    for (int i = 0; i < len; i++){
        ov534_reg_write(handle, data[i][0], data[i][1]);
    }
}

void sccb_w_array(
    libusb_device_handle *handle,
    const uint8_t (*data)[2],
    int len
){
    for (int i = 0; i < len; i++){
        if (data[0][0] != 0xff) {
            sccb_reg_write(handle, data[i][0], data[i][1]);
        }else{
            sccb_reg_read(handle, data[i][1]);
            sccb_reg_write(handle, 0xff, 0x00);
        }
    }
}
