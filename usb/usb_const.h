/******************************************************************************/
/* $Id: usb_typedef.h, 25 Jan 01 $MH$                                         */
/*                                                                            */
/* Copyright (C) 2001, Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/* Symbolic constants used to create USB descriptors, request handler table,  */
/* and decode USB Setup packets                                               */
/*                                                                            */
/******************************************************************************/

#ifndef _USB_CONST_H
#define _USB_CONST_H

/******************************************************************************/
/*                        USB Descriptor Types                                */
/*                                                                            */
/******************************************************************************/

#define USB_DESCRIPTOR_DEVICE          0x01
#define USB_DESCRIPTOR_CONFIG          0x02
#define USB_DESCRIPTOR_STRING          0x03
#define USB_DESCRIPTOR_INTRFC          0x04
#define USB_DESCRIPTOR_ENDPT           0x05
#define USB_DESCRIPTOR_POWER           0x06
#define USB_DESCRIPTOR_HID             0x21



/******************************************************************************/
/*                            USB Requests                                    */
/*                                                                            */
/******************************************************************************/

#define USB_REQUEST_GET_STATUS              0x8000
#define USB_REQUEST_CLEAR_FEATURE           0x0001
#define USB_REQUEST_SET_FEATURE             0x0003
#define USB_REQUEST_SET_ADDRESS             0x0005
#define USB_REQUEST_GET_DESCRIPTOR          0x8006
#define USB_REQUEST_SET_DESCRIPTOR          0x0007
#define USB_REQUEST_GET_CONFIGURATION       0x8008
#define USB_REQUEST_SET_CONFIGURATION       0x0009
#define USB_REQUEST_GET_INTERFACE           0x800A
#define USB_REQUEST_SET_INTERFACE           0x000B
#define USB_REQUEST_SYNC_FRAME              0x800C
#define USB_REQUEST_BULK_ONLY_RESET         0x20FF
#define USB_REQUEST_GET_MAX_LUN             0xA0FE


/******************************************************************************/
/*                        USB Device Classes                                  */
/*                                                                            */
/******************************************************************************/

#define USB_DEVICE_CLASS_RESERVED           0x00
#define USB_DEVICE_CLASS_AUDIO              0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS     0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    0x03
#define USB_DEVICE_CLASS_MONITOR            0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 0x05
#define USB_DEVICE_CLASS_POWER              0x06
#define USB_DEVICE_CLASS_PRINTER            0x07
#define USB_DEVICE_CLASS_STORAGE            0x08
#define USB_DEVICE_CLASS_HUB                0x09
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    0xFF


/******************************************************************************/
/*                        USB Feature Selectors                               */
/*                                                                            */
/******************************************************************************/

#define USB_FEATURE_ENDPOINT_STALL          0x0000
#define USB_FEATURE_REMOTE_WAKEUP           0x0001
#define USB_FEATURE_POWER_D0                0x0002
#define USB_FEATURE_POWER_D1                0x0003
#define USB_FEATURE_POWER_D2                0x0004
#define USB_FEATURE_POWER_D3                0x0005

#endif  /* _USB_CONST_H */
