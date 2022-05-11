/******************************************************************************/
/* usb_req.h, 25 Jan 01 $MH$                                                  */
/*                                                                            */
/* Copyright (C) 2001  Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/* Header file for usb_req.c                                                  */
/*                                                                            */
/******************************************************************************/

#ifndef _USB_REQ_H
#define _USB_REQ_H

//#include "usb_api.h"
#include "usb_const.h"

/******************************************************************************/
/*                                                                            */
/*       return values for USB request handlers                               */
/*                                                                            */
/******************************************************************************/

typedef enum
{
  USB_REQUEST_DONE = 0,      /* Don't call request again until new SETUP */
  USB_REQUEST_STALL,         /* STALL the control endpoint */
  USB_REQUEST_SEND_ACK,      /* Send a 0 length IN packet */
  USB_REQUEST_GET_ACK,       /* Prepare to receive 0 length OUT packet */
  USB_REQUEST_DATA_IN,       /* Notify handler when IN data has been transmitted */
  USB_REQUEST_DATA_OUT       /* Notify handler when OUT data has been received */

} USB_REQUEST_RET;


#define USB_REQUEST_ARGS  USB_DevNum DevNum, USB_SetupStruct *USB_Setup, \
                          USB_EpHandle hInEp, USB_EpHandle hOutEp


/******************************************************************************/
/*                                                                            */
/*       USB standard requests(defined in chap9 of USB 1.1 spec) handlers     */
/*                                                                            */
/******************************************************************************/

USB_REQUEST_RET USB_reqGetDescriptor(USB_REQUEST_ARGS);
USB_REQUEST_RET USB_reqSetAddress(USB_REQUEST_ARGS);
USB_REQUEST_RET USB_reqSetConfiguration(USB_REQUEST_ARGS);
USB_REQUEST_RET USB_reqGetConfiguration(USB_REQUEST_ARGS);
USB_REQUEST_RET USB_reqGetStatus(USB_REQUEST_ARGS);
USB_REQUEST_RET USB_reqClearFeature(USB_REQUEST_ARGS);
USB_REQUEST_RET USB_reqSetFeature(USB_REQUEST_ARGS);

/******************************************************************************/
/*                                                                            */
/*       Some USB vendor specific request handlers                            */
/*                                                                            */
/******************************************************************************/

// USB_REQUEST_RET USB_reqGetMaxLUN(USB_REQUEST_ARGS);

// #ifndef DISABLE_VENDOR_TEST_MESSAGES
// USB_REQUEST_RET USB_reqVendorSetDword(USB_REQUEST_ARGS);
// USB_REQUEST_RET USB_reqVendorGetDword(USB_REQUEST_ARGS);
// #endif

// #ifndef DISABLE_EEPROM_REQUESTS
// USB_REQUEST_RET USB_reqVendorNVWrite(USB_REQUEST_ARGS);
// USB_REQUEST_RET USB_reqVendorNVRead(USB_REQUEST_ARGS);
// #endif

/******************************************************************************/
/*                                                                            */
/*       USB Bootload Request Handler - Vendor specific                       */
/*                                                                            */
/******************************************************************************/
USB_REQUEST_RET USB_reqVendorInitBootLdr(USB_REQUEST_ARGS);
// USB_REQUEST_RET USB_reqVendorConfigReg(USB_REQUEST_ARGS);
// USB_REQUEST_RET USB_reqVendorLoadSec(USB_REQUEST_ARGS);
// USB_REQUEST_RET USB_reqVendorRun(USB_REQUEST_ARGS);
// USB_REQUEST_RET USB_reqVendorGetSecLdStat(USB_REQUEST_ARGS);

/******************************************************************************/
/*                                                                            */
/*       USB unknow request handler                                           */
/*                                                                            */
/******************************************************************************/

USB_REQUEST_RET USB_reqUnknown(USB_REQUEST_ARGS);

/******************************************************************************/
/*                                                                            */
/*             Data Structure for USB Request Table                           */
/*                                                                            */
/******************************************************************************/

typedef struct
{
  Uint16 Request;
  USB_REQUEST_RET (*fpRequestHandler)(USB_REQUEST_ARGS);

} USB_request_struct;


#endif /* _USB_REQ_H */

