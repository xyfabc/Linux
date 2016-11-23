#include "usb_vendor_cmd.h"

/*
 * xusb: Generic USB test program
 * Copyright © 2009-2012 Pete Batard <pete@akeo.ie>
 * Contributions to Mass Storage by Alan Stern.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



#if defined(_WIN32)
#define msleep(msecs) Sleep(msecs)
#else
#include <unistd.h>
#define msleep(msecs) usleep(1000*msecs)
#endif

#if defined(_MSC_VER)
#define snprintf _snprintf
#define putenv _putenv
#endif

#if !defined(bool)
#define bool int
#endif
#if !defined(true)
#define true (1 == 1)
#endif
#if !defined(false)
#define false (!true)
#endif

// Future versions of libusb will use usb_interface instead of interface
// in libusb_config_descriptor => catter for that
#define usb_interface interface

// Global variables
static bool binary_dump = false;
static bool extra_info = false;
static bool force_device_request = false;	// For WCID descriptor queries
static const char* binary_name = NULL;

static int perr(char const *format, ...)
{
    va_list args;
    int r;

    va_start (args, format);
    r = vfprintf(stderr, format, args);
    va_end(args);

    return r;
}

#define ERR_EXIT(errcode) do { perr("   %s\n", libusb_strerror((enum libusb_error)errcode)); return -1; } while (0)
#define CALL_CHECK(fcall) do { r=fcall; if (r < 0) ERR_EXIT(r); } while (0);
#define B(x) (((x)!=0)?1:0)
#define be_to_int32(buf) (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|(buf)[3])

#define RETRY_MAX                     5
#define REQUEST_SENSE_LENGTH          0x12
#define INQUIRY_LENGTH                0x24
#define READ_CAPACITY_LENGTH          0x08

#define HANVON_SCSI_VENDOR_CMD          0xFF
#define HANVON_SCSI_READ_CMD            0x00
#define HANVON_SCSI_WRITE_CMD           0x01



// Mass Storage Requests values. See section 3 of the Bulk-Only Mass Storage Class specifications
#define BOMS_RESET                    0xFF
#define BOMS_GET_MAX_LUN              0xFE

// Section 5.1: Command Block Wrapper (CBW)
struct command_block_wrapper {
    uint8_t dCBWSignature[4];
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
};

// Section 5.2: Command Status Wrapper (CSW)
struct command_status_wrapper {
    uint8_t dCSWSignature[4];
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
};

static uint8_t cdb_length[256] = {
//	 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  0
    06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  1
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  2
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  3
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  4
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  5
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  6
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  7
    16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  8
    16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  9
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  A
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  B
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  C
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  D
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  E
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,10,  //  F
};

static enum test_type {
    USE_GENERIC,
    USE_PS3,
    USE_XBOX,
    USE_SCSI,
    USE_HID,
} test_mode;
const static uint16_t VID = 0x0483, PID = 0x5720;


void display_buffer_hex(unsigned char *buffer, unsigned size)
{
    unsigned i, j, k;

    for (i=0; i<size; i+=16) {
        printf("\n  %08x  ", i);
        for(j=0,k=0; k<16; j++,k++) {
            if (i+j < size) {
                printf("%02x", buffer[i+j]);
            } else {
                printf("  ");
            }
            printf(" ");
        }
        printf(" ");
        for(j=0,k=0; k<16; j++,k++) {
            if (i+j < size) {
                if ((buffer[i+j] < 32) || (buffer[i+j] > 126)) {
                    printf(".");
                } else {
                    printf("%c", buffer[i+j]);
                }
            }
        }
    }
    printf("\n" );
}

static char* uuid_to_string(const uint8_t* uuid)
{
    static char uuid_string[40];
    if (uuid == NULL) return NULL;
    sprintf(uuid_string, "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
        uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7],
        uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    return uuid_string;
}

static int send_mass_storage_command(libusb_device_handle *handle, uint8_t endpoint, uint8_t lun,
    uint8_t *cdb, uint8_t direction, int data_length, uint32_t *ret_tag)
{
    static uint32_t tag = 1;
    uint8_t cdb_len;
    int i, r, size;
    struct command_block_wrapper cbw;

    if (cdb == NULL) {
        return -1;
    }

    if (endpoint & LIBUSB_ENDPOINT_IN) {
        perr("send_mass_storage_command: cannot send command on IN endpoint\n");
        return -1;
    }

    cdb_len = cdb_length[cdb[0]];
    if ((cdb_len == 0) || (cdb_len > sizeof(cbw.CBWCB))) {
        perr("send_mass_storage_command: don't know how to handle this command (%02X, length %d)\n",
            cdb[0], cdb_len);
        return -1;
    }

    memset(&cbw, 0, sizeof(cbw));
    cbw.dCBWSignature[0] = 'U';
    cbw.dCBWSignature[1] = 'S';
    cbw.dCBWSignature[2] = 'B';
    cbw.dCBWSignature[3] = 'C';
    *ret_tag = tag;
    cbw.dCBWTag = tag++;
    cbw.dCBWDataTransferLength = data_length;
    cbw.bmCBWFlags = direction;
    cbw.bCBWLUN = lun;
    // Subclass is 1 or 6 => cdb_len
    cbw.bCBWCBLength = cdb_len;
    memcpy(cbw.CBWCB, cdb, cdb_len);

    i = 0;
    do {
        // The transfer length must always be exactly 31 bytes.
        r = libusb_bulk_transfer(handle, endpoint, (unsigned char*)&cbw, 31, &size, 1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
    if (r != LIBUSB_SUCCESS) {
        perr("   send_mass_storage_command: %s\n", libusb_strerror((enum libusb_error)r));
        return -1;
    }

    //printf("   sent %d CDB bytes\n", cdb_len);
    return 0;
}

static int get_mass_storage_status(libusb_device_handle *handle, uint8_t endpoint, uint32_t expected_tag)
{
    int i, r, size;
    struct command_status_wrapper csw;

    // The device is allowed to STALL this transfer. If it does, you have to
    // clear the stall and try again.
    i = 0;
    do {
        r = libusb_bulk_transfer(handle, endpoint, (unsigned char*)&csw, 13, &size, 1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
    if (r != LIBUSB_SUCCESS) {
        perr("   get_mass_storage_status: %s\n", libusb_strerror((enum libusb_error)r));
        return -1;
    }
    if (size != 13) {
        perr("   get_mass_storage_status: received %d bytes (expected 13)\n", size);
        return -1;
    }
    if (csw.dCSWTag != expected_tag) {
        perr("   get_mass_storage_status: mismatched tags (expected %08X, received %08X)\n",
            expected_tag, csw.dCSWTag);
        return -1;
    }
    // For this test, we ignore the dCSWSignature check for validity...
    //printf("   Mass Storage Status: %02X (%s)\n", csw.bCSWStatus, csw.bCSWStatus?"FAILED":"Success");
    if (csw.dCSWTag != expected_tag)
        return -1;
    if (csw.bCSWStatus) {
        // REQUEST SENSE is appropriate only if bCSWStatus is 1, meaning that the
        // command failed somehow.  Larger values (2 in particular) mean that
        // the command couldn't be understood.
        if (csw.bCSWStatus == 1)
            return -2;	// request Get Sense
        else
            return -1;
    }

    // In theory we also should check dCSWDataResidue.  But lots of devices
    // set it wrongly.
    return 0;
}

static void get_sense(libusb_device_handle *handle, uint8_t endpoint_in, uint8_t endpoint_out)
{
    uint8_t cdb[16];	// SCSI Command Descriptor Block
    uint8_t sense[18];
    uint32_t expected_tag;
    int size;
    int rc;

    // Request Sense
    //printf("Request Sense:\n");
    memset(sense, 0, sizeof(sense));
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = 0x03;	// Request Sense
    cdb[4] = REQUEST_SENSE_LENGTH;

    send_mass_storage_command(handle, endpoint_out, 0, cdb, LIBUSB_ENDPOINT_IN, REQUEST_SENSE_LENGTH, &expected_tag);
    rc = libusb_bulk_transfer(handle, endpoint_in, (unsigned char*)&sense, REQUEST_SENSE_LENGTH, &size, 1000);
    if (rc < 0)
    {
        printf("libusb_bulk_transfer failed: %s\n", libusb_error_name(rc));
        return;
    }
    //printf("   received %d bytes\n", size);

    if ((sense[0] != 0x70) && (sense[0] != 0x71)) {
        perr("   ERROR No sense data\n");
    } else {
        //perr("   ERROR Sense: %02X %02X %02X\n", sense[2]&0x0F, sense[12], sense[13]);
    }
    // Strictly speaking, the get_mass_storage_status() call should come
    // before these perr() lines.  If the status is nonzero then we must
    // assume there's no data in the buffer.  For xusb it doesn't matter.
    get_mass_storage_status(handle, endpoint_in, expected_tag);
}

// Mass Storage device to test bulk transfers (non destructive test)
//static int test_mass_storage(libusb_device_handle *handle, uint8_t endpoint_in, uint8_t endpoint_out)
//{
//    int size;
//    uint32_t expected_tag;
//    uint8_t cdb[16];	// SCSI Command Descriptor Block
//    unsigned char *data;

//    // coverity[tainted_data]
//    data = (unsigned char*) calloc(1, 1);
//    if (data == NULL) {
//        perr("   unable to allocate data buffer\n");
//        return -1;
//    }

//    // Send Read
//    //printf("Attempting to read %d bytes:\n", 9);
//    memset(cdb, 0, sizeof(cdb));

//    cdb[0] = 0xFF;	// Read(10)
//    cdb[1] = 0x00;	// 1 block
//    cdb[2] = 0xB6;	// Read(10)
//    cdb[3] = 0x00;	// 1 block
//    cdb[4] = 0x0A;	// Read(10)


//    send_mass_storage_command(handle, endpoint_out, 0, cdb, LIBUSB_ENDPOINT_IN, 9, &expected_tag);
//    libusb_bulk_transfer(handle, endpoint_in, data, 9, &size, 5000);
//    //printf("   READ: received %d bytes\n", size);
//    if (get_mass_storage_status(handle, endpoint_in, expected_tag) == -2) {
//        get_sense(handle, endpoint_in, endpoint_out);
//        return -1;
//    } else {
//        display_buffer_hex(data, size);
//        return 0;
//    }
//    free(data);

//    return 0;
//}

int SyncCommandWrite(
        libusb_device_handle *handle,
        uint8_t endpoint_in,
        uint8_t endpoint_out,
        unsigned char Commandword,
        unsigned char addr1,
        unsigned char addr2,
        unsigned char num,
        unsigned char *pdata)
{
    int size = 0;
    uint32_t expected_tag;
    uint8_t cdb[16];	// SCSI Command Descriptor Block
    // Send Read
    //printf("Attempting to write %d bytes:%d::%d\n", num,endpoint_in,endpoint_out);
    memset(cdb, 0, sizeof(cdb));

    cdb[0] = HANVON_SCSI_VENDOR_CMD;
    cdb[1] = HANVON_SCSI_WRITE_CMD;
    cdb[2] = Commandword;
    cdb[3] = addr1;
    cdb[4] = addr2;
    cdb[5] = num;


    send_mass_storage_command(handle, endpoint_out, 0, cdb, LIBUSB_ENDPOINT_OUT, num, &expected_tag);
    if(num>0){
        libusb_bulk_transfer(handle, endpoint_out, pdata, num, &size, 5000);
        //printf("  Write: send %d bytes\n", size);

    }else if(0 == num){

    }
    if (get_mass_storage_status(handle, endpoint_in, expected_tag) == -2) {
        get_sense(handle, endpoint_in, endpoint_out);
        return -1;
    } else {
       // display_buffer_hex(pdata, size);
        return size;
    }


    return 0;
}

//----------------------------------------------------------------
int SyncCommandRead(
        libusb_device_handle *handle,
        uint8_t endpoint_in,
        uint8_t endpoint_out,
        unsigned char Commandword,
        unsigned char addr1,
        unsigned char addr2,
        unsigned char num,
        unsigned char *pdata)
{
    int size;
    uint32_t expected_tag;
    uint8_t cdb[16];	// SCSI Command Descriptor Block
    // Send Read
   // printf("Attempting to read %d bytes:\n", num);
    memset(cdb, 0, sizeof(cdb));

    cdb[0] = HANVON_SCSI_VENDOR_CMD;
    cdb[1] = HANVON_SCSI_READ_CMD;
    cdb[2] = Commandword;
    cdb[3] = addr1;
    cdb[4] = addr2;
    cdb[5] = num;


    send_mass_storage_command(handle, endpoint_out, 0, cdb, LIBUSB_ENDPOINT_IN, num, &expected_tag);
    libusb_bulk_transfer(handle, endpoint_in, pdata, num, &size, 5000);
   // printf("   READ: received %d bytes\n", size);
    if (get_mass_storage_status(handle, endpoint_in, expected_tag) == -2) {
        get_sense(handle, endpoint_in, endpoint_out);
         //printf("   READ: err\n", size);
        return -1;
    } else {
       // display_buffer_hex(pdata, size);
        return size;
    }

    return size;
}

static int init_libusb()
{
    const struct libusb_version* version;
    int r;
    uint16_t endian_test = 0xBE00;


    if (((uint8_t*)&endian_test)[0] == 0xBE) {
        printf("Despite their natural superiority for end users, big endian\n"
            "CPUs are not supported with this program, sorry.\n");
        return 0;
    }
    version = libusb_get_version();
    printf("Using libusb v%d.%d.%d.%d\n\n", version->major, version->minor, version->micro, version->nano);
    r = libusb_init(NULL);
    if (r < 0){
        return r;
    }

    return 0;

}
static int init_usbdev(const uint16_t vid, const uint16_t pid,
                libusb_device_handle **handle,
                uint8_t *endpoint_in,
                uint8_t *endpoint_out,
                int *nb_ifaces)
{
    libusb_device *dev;
    struct libusb_config_descriptor *conf_desc;
    const struct libusb_endpoint_descriptor *endpoint;
    int i, j, k, r;
    int iface,first_iface = -1;

    *nb_ifaces = -1;
    *endpoint_in = 0;
    *endpoint_out = 0;

    printf("Opening device %04X:%04X...\n", vid, pid);
    *handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (*handle == NULL) {
        perr("  Failed.\n");
        return -1;
    }
    dev = libusb_get_device(*handle);
    CALL_CHECK(libusb_get_config_descriptor(dev, 0, &conf_desc));
    *nb_ifaces = conf_desc->bNumInterfaces;
    if (*nb_ifaces > 0)
        first_iface = conf_desc->usb_interface[0].altsetting[0].bInterfaceNumber;
    for (i=0; i<*nb_ifaces; i++) {
        for (j=0; j<conf_desc->usb_interface[i].num_altsetting; j++) {

            for (k=0; k<conf_desc->usb_interface[i].altsetting[j].bNumEndpoints; k++) {

                endpoint = &conf_desc->usb_interface[i].altsetting[j].endpoint[k];
                if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK | LIBUSB_TRANSFER_TYPE_INTERRUPT)) {
                    if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                        if (!*endpoint_in)
                            *endpoint_in = endpoint->bEndpointAddress;
                    } else {
                        if (!*endpoint_out)
                            *endpoint_out = endpoint->bEndpointAddress;
                    }
                }
            }
        }
    }
    libusb_free_config_descriptor(conf_desc);

    libusb_set_auto_detach_kernel_driver(*handle, 1);
    for (iface = 0; iface < *nb_ifaces; iface++)
    {
        printf("\nClaiming interface %d...%d\n", iface,*nb_ifaces);
        r = libusb_claim_interface(*handle, iface);
        if (r != LIBUSB_SUCCESS) {
            perr("   Failed.\n");
        }
    }
    return 0;

}
static int exit_usbdev( libusb_device_handle *handle,const int nb_ifaces)
{
    int iface;
    for (iface = 0; iface<nb_ifaces; iface++) {
        printf("Releasing interface %d...%d\n", iface,nb_ifaces);
        libusb_release_interface(handle, iface);
    }

    printf("Closing device...\n");
    libusb_close(handle);

    return 0;
}
static int exit_libusb()
{
    libusb_exit(NULL);
    return 0;
}

int init_usb(const uint16_t vid, const uint16_t pid,
             libusb_device_handle **handle,
             uint8_t *endpoint_in,
             uint8_t *endpoint_out,
             int *nb_ifaces)
{
    int ret = -1;
  ret = init_libusb();
  if(ret<0){
      return -1;
  }
  return init_usbdev(vid,pid,
                  handle,
                  endpoint_in,
                  endpoint_out,
                  nb_ifaces);
}
int exit_usb(libusb_device_handle *handle,const int nb_ifaces)
{
   int ret = -1;
   ret = exit_usbdev(handle,nb_ifaces);
   if(ret<0){
       return -1;
   }
   return exit_libusb();
}
