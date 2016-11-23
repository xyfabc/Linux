#include "usb_vendor_cmd.h"

const static uint16_t VID = 0x0483, PID = 0x5720;

//this is the demo
int main(int argc, char** argv)
{
    libusb_device_handle *handle;
    uint8_t endpoint_in = 0;
    uint8_t endpoint_out = 0;
    int nb_ifaces = -1;
    static int data = 0x00;
    unsigned char buf_send[128] = {0x22};
    unsigned char buf_rec[128] = {0x22};
    int ret = -1;

    init_usb(VID,PID,&handle,&endpoint_in,&endpoint_out,&nb_ifaces);

    for(int i=0;i<10;i++){
        buf_send[0] = data++;
        buf_send[1] = data++;
        ret = SyncCommandWrite(handle, endpoint_in, endpoint_out,0xB4,0x00,0x0A,2,buf_send);
        if(ret>0){
            display_buffer_hex(buf_send, ret);
        }

        usleep(10000);   //delay needed
        ret = SyncCommandRead(handle, endpoint_in, endpoint_out,0xB6,0x00,0x0A,9,buf_rec);
        if(ret>0){
            display_buffer_hex(buf_rec, ret);
        }
        printf("the %d\n",i);
        if(buf_rec[0] == buf_send[0]){
          printf("test OK!!\n");
        }
    }

    exit_usb(handle,nb_ifaces);

    return 0;
}
