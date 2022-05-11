/******************************************************************************/
/* Copyright (C) 2001  Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/*  File Name   : usb_dscr.c                                                  */
/*  Project     : TMS320VC5509 USB Module Support                             */
/*  Author      : C5000 H/W Applications                                      */
/*                                                                            */
/*  Version     : 0.1                                                         */
/*  Date        : 30 Apr 2001                                                 */
/*                                                                            */
/*  Description : USB descriptor example file.                                */
/*                This file shows how to create USB descriptors using the     */
/*                resources supported by the USB driver.                      */
/*                                                                            */
/*                Users can add/modify the descriptors as required by their   */
/*                application                                                 */
/*                                                                            */
/*  Updates     :                                                             */
/*                                                                            */
/*  05/17/01 $MH$                                                             */
/*                                                                            */
/*  Added endpoint 4 in and out, and endpoint 5 in for host dma mode and      */
/*  iso support                                                               */
/*                                                                            */
/******************************************************************************/
/*
 *  USB Test Component Copyright (C) 2004, Spectrum Digital, Inc.  All Rights Reserved.
 *
 *  Modified the fields idVendor & idProduct in device_descriptor[] to be used 
 *  in conjunction with an external usb test.
 *
 */
 
 
#include "csl.h"
#include "csl_usb.h"
#include "usb_const.h"


//  Descriptors are stored in little endian format, because that is the way
//  USB sends data.  The comments are listed by what goes out first on the bus


/******************************************************************************/
/*                                                                            */
/*                             Device Descriptor                              */
/*                                                                            */
/******************************************************************************/

const Uint16 device_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_DEVICE<<8) | 18,      // bLength, bDescriptorType
  0x0101,                               // bcdUSB
  0x0000,                               // bDeviceClass, bDeviceSubClass
  0x4000,                               // bDeviceProtocol, bMaxPacketSize0 = 64
  0x0C55,                               // idVendor = Spectrum Digital
  0x1234,                               // idProduct = F000
  0x0000,                               // bcdDevice ID = prototype
  0x0201,                               // iManufacturer, iProductName
  0x0103                                // iSerialNumber, bNumConfigurations
};

/******************************************************************************/
/*                                                                            */
/*                         Configuration descriptor                           */
/*                                                                            */
/******************************************************************************/

const Uint16 configuration_descriptor[] =
{
  0x0000,                                        // field for xfer_byte_cnt - used by the data
                                                 // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_CONFIG<<8) | 9,                // bLength, bDescriptorType
  32,                                            // wTotalLength = 9+9+7+7+7+7+7+7
  0x0101,                                        // bNumInterfaces, bConfigurationValue
  0x6004,                                        // iConfiguration, bmAttributes = self pwr, no rwu
  0                                              // bMaxPower = none
};

/******************************************************************************/
/*                                                                            */
/*                              Interface descriptor                          */
/*                                                                            */
/******************************************************************************/

const Uint16 usb_demo_interface_descriptor[] =
{
  0x0000,                                 // field for xfer_byte_cnt - used by the data
                                          // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_INTRFC<<8) | 9,         // bLength, bDescriptorType
  0x0000,                                 // bInterfaceNumber, bAlternateSetting
  0xff02,                                 // bNumEndpoints = 6, bInterfaceClass
  0x0000,                                 // bInterfaceSubClass, bInterfaceProtocol
  0x05                                    // iInterface = index in string descriptor
};


/******************************************************************************/
/*                                                                            */
/*                              Endpoint descriptors                          */
/*                                                                            */
/******************************************************************************/

// bulk endpoint descriptor - endpt2 OUT
const Uint16 usb_demo_bulk_out_endpoint_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_ENDPT<<8) | 7,        // bLength, bDescriptorType
  0x0202,                               // bEndpointAddress = 2 OUT, bmAttributes = bulk
  0x0040,                               // wMaxPacketSize
  0x00                                  // bInterval
};

// bulk endpoint descriptor - endpt2 IN
const Uint16 usb_demo_bulk_in_endpoint_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_ENDPT<<8) | 7,        // bLength, bDescriptorType */
  0x0282,                               // bEndpointAddress = 2 IN , bmAttributes
  0x0040,                               // wMaxPacketSize
  0x00                                  // bInterval
};

// interrupt endpoint descriptor - endpt3 IN
const Uint16 usb_demo_intrpt_in_endpoint_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_ENDPT<<8) | 7,        // bLength, bDescriptorType
  0x0383,                               // bEndpointAddress = 3 IN , bmAttributes = intrpt
  0x0040,                               // wMaxPacketSize
  0x01                                  // bInterval = once per mSec
};

// bulk endpoint descriptor - endpt4 OUT (for host dma mode )
const Uint16 usb_demo_hostdma_out_endpoint_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_ENDPT<<8) | 7,        // bLength, bDescriptorType
  0x0204,                               // bEndpointAddress = 4 OUT, bmAttributes = bulk
  0x0040,                               // wMaxPacketSize
  0x00                                  // bInterval
};

// bulk endpoint descriptor - endpt4 IN (for host dma mode )
const Uint16 usb_demo_hostdma_in_endpoint_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_ENDPT<<8) | 7,        // bLength, bDescriptorType */
  0x0284,                               // bEndpointAddress = 4 IN , bmAttributes
  0x0040,                               // wMaxPacketSize
  0x00                                  // bInterval
};

// iso endpoint descriptor - endpt5 IN
const Uint16 usb_demo_iso_in_endpoint_descriptor[] =
{
  0x0000,                               // field for xfer_byte_cnt - used by the data
                                        // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_ENDPT<<8) | 7,        // bLength, bDescriptorType
  0x0185,                               // bEndpointAddress = 5 IN , bmAttributes = iso
  0x0010,                               // wMaxPacketSize
  0x01                                  // bInterval = once per mSec
};

/******************************************************************************/
/*                                                                            */
/*                       Descriptor Linked List                               */
/*                                                                            */
/******************************************************************************/

// Create a linked list of all the descriptors (except for device and string descr)
// The framework expects the configuration and the interface descriptor as a linked
// list

USB_DataStruct usb_demo_iso_in_endpoint_descriptor_link =
{
  7,                                                   // length in bytes
  (Uint16 *)&usb_demo_iso_in_endpoint_descriptor[0],   // pointer to this descriptor
  NULL                                                 // end of descriptor link
};

USB_DataStruct usb_demo_hostdma_in_endpoint_descriptor_link =
{
  7,                                                     // length in bytes
  (Uint16 *)&usb_demo_hostdma_in_endpoint_descriptor[0], // pointer to this descriptor
  &usb_demo_iso_in_endpoint_descriptor_link              // pointer to next descriptor link
};

USB_DataStruct usb_demo_hostdma_out_endpoint_descriptor_link =
{
  7,                                                      // length in bytes
  (Uint16 *)&usb_demo_hostdma_out_endpoint_descriptor[0], // pointer to this descriptor
  &usb_demo_hostdma_in_endpoint_descriptor_link           // pointer to next descriptor link
};


USB_DataStruct usb_demo_intrpt_in_endpoint_descriptor_link =
{
  7,                                                    // length in bytes
  (Uint16 *)&usb_demo_intrpt_in_endpoint_descriptor[0], // pointer to this descriptor
  &usb_demo_hostdma_out_endpoint_descriptor_link        // pointer to next descriptor link
};

USB_DataStruct usb_demo_bulk_in_endpoint_descriptor_link =
{
  7,                                                   // length in bytes
  (Uint16 *)&usb_demo_bulk_in_endpoint_descriptor[0],  // pointer to this descriptor
  NULL/*&usb_demo_intrpt_in_endpoint_descriptor_link*/         // pointer to next descriptor link
};

USB_DataStruct usb_demo_bulk_out_endpoint_descriptor_link =
{
  7,                                                   // length in bytes
  (Uint16 *)&usb_demo_bulk_out_endpoint_descriptor[0], // pointer to this descriptor
  &usb_demo_bulk_in_endpoint_descriptor_link           // pointer to next descriptor link
};

USB_DataStruct usb_demo_interface_descriptor_link =
{
  9,                                                   // length in bytes
  (Uint16 *)&usb_demo_interface_descriptor[0],         // pointer to this descriptor
  &usb_demo_bulk_out_endpoint_descriptor_link          // pointer to next descriptor link
};

USB_DataStruct configuration_descriptor_link =
{
  9,                                                   // length in bytes
  (Uint16 *)&configuration_descriptor[0],              // pointer to this descriptor
  &usb_demo_interface_descriptor_link                  // pointer to next descriptor link
};

/******************************************************************************/
/*                                                                            */
/*                       String Descriptors                                   */
/*                                                                            */
/******************************************************************************/

Uint16 string_descriptor_langid[] =
{
  0x0000,                           // field for xfer_byte_cnt - used by the data
                                    // transfer API, not an integral part of descriptor
  (USB_DESCRIPTOR_STRING<<8) | 4,   // bLength, bDescriptorType
  0x0409,                           // LANGID (English)
  0x0000
};


// C55xx chars are 16-bit
// calculate and add the length to the beginning of the strings
// at runtime before sending the string descriptors

char *string_descriptor[] =
{
  (char *)&string_descriptor_langid[0],  // LANGID
  "  Spectrum Digital, Inc.",            // iManufacturer
  "  5509A USB Test",                    // iProductName
  "  5509A USB Test",                    // iConfiguration
  "  Vendor Specific",                   // iInterface - Vendor Specific
  NULL                                   // end of string descriptor
};

