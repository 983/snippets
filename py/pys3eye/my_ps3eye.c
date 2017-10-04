#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DLL_PREFIX __declspec(dllexport) __attribute__((__stdcall__))

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    
    return t.tv_sec + 1e-9*t.tv_nsec;
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void sleep_seconds(double sec){
    Sleep(1000*sec);
}

#include <stdio.h>
#define WHERE printf("File: %s, Line: %i, Func: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

#include <assert.h>

#include "libusb.h"

#include "magic_constants.h"

#include "write_usb.c"
#include "write_usb_camera.c"

#include "debayer.c"

#include "my_list.h"

/* packet types when moving from iso buf to frame buf */
enum gspca_packet_type {
    DISCARD_PACKET,
    FIRST_PACKET,
    INTER_PACKET,
    LAST_PACKET
};

struct Buffer {
    uint8_t *data;
    int size;
};

CRITICAL_SECTION buffer_lock;

struct my_list buffers[1];

struct my_list complete_frames[1];

int frame_size = 640*480;

int last_pts = 0;
int last_fid = 0;
int cur_frame_data_len = 0;
enum gspca_packet_type  last_packet_type;
uint8_t *cur_frame_start;
int payload_errors = 0;

/*
 * look for an input transfer endpoint in an alternate setting
 * libusb_endpoint_descriptor
 */
static uint8_t find_ep(struct libusb_device *device)
{
    const struct libusb_interface_descriptor *altsetting = NULL;
    const struct libusb_endpoint_descriptor *ep;
    struct libusb_config_descriptor *config = NULL;
    int i;
    uint8_t ep_addr = 0;

    libusb_get_active_config_descriptor(device, &config);

    if (!config) return 0;

    for (i = 0; i < config->bNumInterfaces; i++) {
        altsetting = config->interface[i].altsetting;
        if (altsetting[0].bInterfaceNumber == 0) {
            break;
        }
    }

    for (i = 0; i < altsetting->bNumEndpoints; i++) {
        ep = &altsetting->endpoint[i];
        if ((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK 
            && ep->wMaxPacketSize != 0) 
        {
            ep_addr = ep->bEndpointAddress;
            break;
        }
    }

    libusb_free_config_descriptor(config);

    return ep_addr;
}


void frame_add(enum gspca_packet_type packet_type, const uint8_t *data, int len)
{
    if (packet_type == FIRST_PACKET) 
    {
        cur_frame_data_len = 0;
    } 
    else
    {
        switch(last_packet_type)  // ignore warning.
        {
            case DISCARD_PACKET:
                if (packet_type == LAST_PACKET) {
                    last_packet_type = packet_type;
                    cur_frame_data_len = 0;
                }
                return;
            case LAST_PACKET:
                return;
            default:
                break;
        }
    }

    /* append the packet to the frame buffer */
    if (len > 0)
    {
        if(cur_frame_data_len + len > frame_size)
        {
            packet_type = DISCARD_PACKET;
            cur_frame_data_len = 0;
        } else {
            memcpy(cur_frame_start+cur_frame_data_len, data, len);
            cur_frame_data_len += len;
        }
    }

    last_packet_type = packet_type;

    if (packet_type == LAST_PACKET){
        //printf("frame completed, cur_frame_data_len: %i\n", cur_frame_data_len);
        cur_frame_data_len = 0;
        
        my_list_push_tail(complete_frames, cur_frame_start);
        
        cur_frame_start = (uint8_t*)malloc(640*480);
        //cur_frame_start = frame_queue->Enqueue();
        //debug("frame completed %d\n", frame_complete_ind);
    }
}

void pkt_scan(uint8_t *data, int len)
{
    uint32_t this_pts;
    uint16_t this_fid;
    int remaining_len = len;
    int payload_len;

    payload_len = 2048; // bulk type
    do {
        len = remaining_len < payload_len ? remaining_len : payload_len;

        /* Payloads are prefixed with a UVC-style header.  We
           consider a frame to start when the FID toggles, or the PTS
           changes.  A frame ends when EOF is set, and we've received
           the correct number of bytes. */

        /* Verify UVC header.  Header length is always 12 */
        if (data[0] != 12 || len < 12) {
            printf("bad header\n");
            goto discard;
        }

        /* Check errors */
        if (data[1] & UVC_STREAM_ERR) {
            payload_errors++;
            goto discard;
        }

        /* Extract PTS and FID */
        if (!(data[1] & UVC_STREAM_PTS)) {
            printf("PTS not present\n");
            goto discard;
        }

        this_pts = (data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2];
        this_fid = (data[1] & UVC_STREAM_FID) ? 1 : 0;

        /* If PTS or FID has changed, start a new frame. */
        if (this_pts != last_pts || this_fid != last_fid) {
            
            if (last_packet_type == INTER_PACKET)
            {
                
                /* The last frame was incomplete, so don't keep it or we will glitch */
                frame_add(DISCARD_PACKET, NULL, 0);
            }
            last_pts = this_pts;
            last_fid = this_fid;
            frame_add(FIRST_PACKET, data + 12, len - 12);
        } /* If this packet is marked as EOF, end the frame */
        else if (data[1] & UVC_STREAM_EOF) 
        {
            
            last_pts = 0;
            if(cur_frame_data_len + len - 12 != frame_size)
            {
                
                goto discard;
            }
            frame_add(LAST_PACKET, data + 12, len - 12);
        } else {
            
            /* Add the data from this payload */
            frame_add(INTER_PACKET, data + 12, len - 12);
        }

        /* Done this payload */
        goto scan_next;

discard:
        /* Discard data until a new frame starts. */
        frame_add(DISCARD_PACKET, NULL, 0);
scan_next:
        remaining_len -= len;
        data += len;
    } while (remaining_len > 0);
}

static void LIBUSB_CALL transfer_completed_callback(struct libusb_transfer *xfr)
{
    enum libusb_transfer_status status = xfr->status;

    if (status != LIBUSB_TRANSFER_COMPLETED) 
    {
        printf("ERROR: transfer status %d\n", status);

        libusb_free_transfer(xfr);
        //urb->transfer_canceled();
        
        if(status != LIBUSB_TRANSFER_CANCELLED)
        {
            //urb->close_transfers();
        }
        return;
    }

    //debug("length:%u, actual_length:%u\n", xfr->length, xfr->actual_length);

    struct Buffer *buffer = malloc(sizeof(*buffer));
    
    buffer->data = xfr->buffer;
    buffer->size = xfr->actual_length;
    
    xfr->buffer = (uint8_t*)malloc(TRANSFER_SIZE);
    
    EnterCriticalSection(&buffer_lock);
    
    my_list_push_tail(buffers, buffer);
    
    LeaveCriticalSection(&buffer_lock);
    
    //pkt_scan(xfr->buffer, xfr->actual_length);
    
    if (libusb_submit_transfer(xfr) < 0) {
        printf("error re-submitting URB\n");
        //urb->close_transfers();
    }
}

int find_camera_devices(
    libusb_context *usb_context,
    libusb_device **camera_devices,
    int max_cameras
){
    int n_cameras = 0;
    
    libusb_device *dev;
    libusb_device **devs;
    libusb_device_handle *devhandle;
    int i = 0;
    int cnt;

    cnt = (int)libusb_get_device_list(usb_context, &devs);

    if (cnt < 0) {
        printf("Error Device scan\n");
    }

    cnt = 0;
    while ((dev = devs[i++]) != NULL) 
    {
        struct libusb_device_descriptor desc;
        libusb_get_device_descriptor(dev, &desc);
        if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID)
        {
            int err = libusb_open(dev, &devhandle);
            if (err == 0)
            {
                libusb_close(devhandle);
                
                printf("found a device\n");
                
                if (n_cameras < max_cameras){
                    camera_devices[n_cameras++] = dev;
                }
                
                libusb_ref_device(dev);
                cnt++;
            }
        }
    }
    
    libusb_free_device_list(devs, 1);
    
    return n_cameras;
}

DWORD WINAPI usb_transfer_worker_thread(void *user_data){
    printf("usb transfer thread started\n");
    
    HANDLE current_thread = GetCurrentThread();
    SetPriorityClass(current_thread, REALTIME_PRIORITY_CLASS);
    SetThreadPriority(current_thread, THREAD_PRIORITY_HIGHEST);
    SetThreadPriority(current_thread, THREAD_PRIORITY_TIME_CRITICAL);
    
    libusb_context *usb_context = (libusb_context*)user_data;
    
    while (1){
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 50 * 1000;
    
        libusb_handle_events_timeout_completed(usb_context, &tv, NULL);
    }
    
    return 0;
}

DLL_PREFIX
void start_camera(void){
    printf("start camera\n");
    
    struct libusb_context *usb_context;
    libusb_init(&usb_context);
    libusb_set_debug(usb_context, 1);
    
    libusb_device *device_;
    
    find_camera_devices(usb_context, &device_, 1);
    
    libusb_device_handle *handle_;

    // open, set first config and claim interface
    int res = libusb_open(device_, &handle_);
    
    if (res != 0){
        printf("libusb_open failed\n");
    }
    
    
    res = libusb_claim_interface(handle_, 0);
    
    if(res != 0) {
        printf("libusb_claim_interface failed\n");
    }
    
    int frame_width = 640;
    int frame_height = 480;
    
    //frame_rate = ov534_set_frame_rate(handle_, desiredFrameRate, true);
    
    // TODO allow different frame rate
    int frame_rate = 60;
    
    /* reset bridge */
    ov534_reg_write(handle_, 0xe7, 0x3a);
    ov534_reg_write(handle_, 0xe0, 0x08);

    sleep_seconds(0.1);

    /* initialize the sensor address */
    ov534_reg_write(handle_, OV534_REG_ADDRESS, 0x42);
    
    /* reset sensor */
    sccb_reg_write(handle_, 0x12, 0x80);
    
    sleep_seconds(0.1);
    
    /* probe the sensor */
    sccb_reg_read(handle_, 0x0a);
    uint16_t sensor_id = sccb_reg_read(handle_, 0x0a) << 8;
    sccb_reg_read(handle_, 0x0b);
    sensor_id |= sccb_reg_read(handle_, 0x0b);
    printf("Sensor ID: %04x\n", sensor_id);
    
    /* initialize */
    reg_w_array(handle_, ov534_reg_initdata, ARRAY_SIZE(ov534_reg_initdata));
    ov534_set_led(handle_, 1);
    sccb_w_array(handle_, ov772x_reg_initdata, ARRAY_SIZE(ov772x_reg_initdata));
    ov534_reg_write(handle_, 0xe0, 0x09);
    ov534_set_led(handle_, 0);
    
    int fps = 60;
    int width = 640;
    int height = 480;
    
    cur_frame_start = (uint8_t*)malloc(640*480);
    
    if (frame_width == 320) {   /* 320x240 */
        reg_w_array(handle_, bridge_start_qvga, ARRAY_SIZE(bridge_start_qvga));
        sccb_w_array(handle_, sensor_start_qvga, ARRAY_SIZE(sensor_start_qvga));
    } else {        /* 640x480 */
        reg_w_array(handle_, bridge_start_vga, ARRAY_SIZE(bridge_start_vga));
        sccb_w_array(handle_, sensor_start_vga, ARRAY_SIZE(sensor_start_vga));
    }
    
    ov534_set_frame_rate(handle_, frame_rate);

    int autogain = 0;
    uint8_t gain = 20;
    uint8_t exposure = 120;
    uint8_t sharpness = 0;
    uint8_t hue = 143;
    int awb = 0;
    uint8_t brightness = 20;
    uint8_t contrast =  37;
    uint8_t blueblc = 128;
    uint8_t redblc = 128;
    uint8_t greenblc = 128;
    int flip_h = 0;
    int flip_v = 0;

    setAutogain(handle_, autogain, gain, exposure);
    setAutoWhiteBalance(handle_, awb);
    setGain(handle_, gain);
    setHue(handle_, hue);
    setExposure(handle_, exposure);
    setBrightness(handle_, brightness);
    setContrast(handle_, contrast);
    setSharpness(handle_, sharpness);
    setRedBalance(handle_, redblc);
    setBlueBalance(handle_, blueblc);
    setGreenBalance(handle_, greenblc);
    setFlip(handle_, flip_h, flip_v);
    
    ov534_set_led(handle_, 1);
    ov534_reg_write(handle_, 0xe0, 0x00); // start stream
    
    
    
    int cur_frame_size = frame_size;

    // Find the bulk transfer endpoint
    uint8_t bulk_endpoint = find_ep(libusb_get_device(handle_));
    
    printf("bulk_endpoint: %i\n", (int)bulk_endpoint);
    
    int ret = libusb_clear_halt(handle_, bulk_endpoint);
    
    if (ret != 0){
        printf("libusb_clear_halt failed\n");
    }
    
    struct libusb_transfer *xfr[NUM_TRANSFERS];
    
    for (int index = 0; index < NUM_TRANSFERS; ++index)
    {
        uint8_t *transfer_buffer = (uint8_t*)calloc(1, TRANSFER_SIZE);
        
        // Create & submit the transfer
        xfr[index] = libusb_alloc_transfer(0);
        
        libusb_fill_bulk_transfer(
            xfr[index],
            handle_,
            bulk_endpoint,
            transfer_buffer,
            TRANSFER_SIZE,
            transfer_completed_callback,
            NULL,
            0);

        int result = libusb_submit_transfer(xfr[index]);
        
        if (result != 0){
            printf("libusb_submit_transfer failed\n");
        }
    }

    last_pts = 0;
    last_fid = 0;
    
    InitializeCriticalSection(&buffer_lock);
    
    CreateThread(NULL, 0, usb_transfer_worker_thread, (void*)usb_context, 0, NULL);
}

DLL_PREFIX
void* get_frame(void){
    struct my_list buffers_copy[1];
    
    {
        /* only do little work between locks */
        /* otherwise we might lose data from the usb transfers */
        EnterCriticalSection(&buffer_lock);
        
        /* copy buffers */
        buffers_copy[0] = buffers[0];
        /* clear buffers */
        my_list_init(buffers);
        
        LeaveCriticalSection(&buffer_lock);
    }
    
    while (!my_list_empty(buffers_copy)){
        struct Buffer *buffer = my_list_pop_head(buffers_copy);
        
        pkt_scan(buffer->data, buffer->size);
        
        free(buffer->data);
        free(buffer);
    }
    
    int width = 640;
    int height = 480;
    
    if (my_list_empty(complete_frames)) return NULL;
    
    uint8_t *raw_pixels = my_list_pop_head(complete_frames);
    
    uint8_t *rgb = (uint8_t*)malloc(width*height*3);
        
    DebayerRGB(width, height, raw_pixels, rgb, 0);
    
    free(raw_pixels);
    
    printf("return %p\n", (void*)rgb);
    
    return rgb;
}

DLL_PREFIX
void free_frame(void *frame){
    printf("free %p\n", frame);
    free(frame);
}

/*
int main(){

    start_camera();

    while (1){
        Sleep(1);

        void *frame = get_frame();
        
        if (frame != NULL){
            printf("got frame\n");
            
            int width = 640;
            int height = 480;
            FILE *fp = fopen("frame.ppm", "wb");
            fprintf(fp, "P6\n%d %d\n255\n", width, height);
            fwrite(frame, 1, width*height*3, fp);
            fclose(fp);
            
            free_frame(frame);
        }
        
        if (payload_errors > 0){
            printf("%i payload errors\n", payload_errors);
            payload_errors = 0;
        }
    }
    
    return 0;
}
*/
