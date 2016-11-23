#ifndef USB_VENDOR_CMD_H
#define USB_VENDOR_CMD_H

#include "./include/libusb-1.0/libusb.h"

/*
 * this lib modified from xusb.c of libusb.
 * you can modify it in accordance with this library on different platforms(andorid.window..),
*/


//init usb
extern int init_usb(const uint16_t vid, const uint16_t pid,
             libusb_device_handle **handle,
             uint8_t *endpoint_in,
             uint8_t *endpoint_out,
             int *nb_ifaces);

//exit usb
extern int exit_usb(libusb_device_handle *handle,const int nb_ifaces);


//receive datas
extern unsigned char SyncCommandRead(
        libusb_device_handle *handle,
        uint8_t endpoint_in,
        uint8_t endpoint_out,
        unsigned char Commandword,
        unsigned char addr1,
        unsigned char addr2,
        unsigned char num,
        unsigned char *pdata);


//send datas
extern unsigned char SyncCommandWrite(
        libusb_device_handle *handle,
        uint8_t endpoint_in,
        uint8_t endpoint_out,
        unsigned char Commandword,
        unsigned char addr1,
        unsigned char addr2,
        unsigned char num,
        unsigned char *pdata);


extern void display_buffer_hex(unsigned char *buffer, unsigned size);

#endif // USB_VENDOR_CMD_H
