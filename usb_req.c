/******************************************************************************/
/* Copyright (C) 2001  Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/*  File Name   : usb_req.c                                                   */
/*  Project     : TMS320VC5509 USB Module Support                             */
/*  Author      : C5000 H/W Applications                                      */
/*                                                                            */
/*  Version     : 0.1                                                         */
/*  Date        : 30 Apr 2001                                                 */
/*  Updated     :                                                             */
/*                                                                            */
/*  Description : USB Chapter 9 standard device request handlers.             */
/*                Users can expand the USB request handling capabilities by   */
/*                adding new members in the USB request table and supplying   */
/*                associated request handler routines.                        */
/*                                                                            */
/******************************************************************************/
/*
 *  USB Test Component Copyright (C) 2004, Spectrum Digital, Inc.  All Rights Reserved.
 */

#include "csl.h"
#include "csl_usb.h"
#include "string.h"
#include "usb_req.h"


Uint16 usbCurDevStat    = 0x01;  // Self Powered
Uint16 usbCurConfigStat = 0x00;  // Device start with config = 0
Uint16 usbCurIntrfcStat = 0x00;  // Only interface 0 supported
Uint16 gen_purpose_buffer[5];    // first 2 bytes for xfer byte count
                                 // next 8 bytes are for usb data

/******************************************************************************/
/*                                                                            */
/*       USB descriptors defined in usb_catdscr.c                             */
/*                                                                            */
/******************************************************************************/
extern const Uint16 device_descriptor[];
extern const Uint16 configuration_descriptor[];
extern USB_DataStruct configuration_descriptor_link;
extern Uint16 string_descriptor_langid[];
extern char *string_descriptor[];




/* Mass Storage Class Request */
#define USB_GET_MAX_LUN             0x80FE
#define USB_MASS_STORAGE_RESET      0xFF

USB_REQUEST_RET USB_reqMassStorageReset(USB_REQUEST_ARGS)
{
    /* ======== USB_MASS_STORAGE_RESET ======== */
    /* Respond with no data */
    USB_postTransaction(hInEp, 0, 0, USB_IOFLAG_NONE|USB_IOFLAG_NOSHORT);

    return(USB_REQUEST_GET_ACK);
}
USB_REQUEST_RET USB_reqGetMaxLun(USB_REQUEST_ARGS)
{
    /* ======== USB_GET_MAX_LUN ======== */
    /* Respond by return N - 1 logical units */

    gen_purpose_buffer[1] = 0;
    USB_postTransaction(hInEp, 1, (void *)&gen_purpose_buffer, USB_IOFLAG_NONE|USB_IOFLAG_NOSHORT);

    return(USB_REQUEST_GET_ACK);
}

/******************************************************************************/
/*                                                                            */
/*       USB Request Table.  USB control(enpoint0) parse through this         */
/*       table and calls the routine that matches the request sent by         */
/*       the host.                                                            */
/*                                                                            */
/******************************************************************************/

USB_request_struct USB_ReqTable[] =
{
    { USB_REQUEST_GET_STATUS         , USB_reqGetStatus },
    { USB_REQUEST_CLEAR_FEATURE      , USB_reqClearFeature },
    { USB_REQUEST_SET_FEATURE        , USB_reqSetFeature },
    { USB_REQUEST_SET_ADDRESS        , USB_reqSetAddress },
    { USB_REQUEST_GET_DESCRIPTOR     , USB_reqGetDescriptor },
    { USB_REQUEST_SET_DESCRIPTOR     , USB_reqUnknown },         // not supported
    { USB_REQUEST_GET_CONFIGURATION  , USB_reqGetConfiguration },
    { USB_REQUEST_SET_CONFIGURATION  , USB_reqSetConfiguration },
    { USB_REQUEST_GET_INTERFACE      , USB_reqUnknown },         // not supported
    { USB_REQUEST_SET_INTERFACE      , USB_reqUnknown },         // not supported
    { USB_REQUEST_SYNC_FRAME         , USB_reqUnknown },         // not supported

//
// place holder for adding more request handlers
//

    { USB_GET_MAX_LUN                   , USB_reqGetMaxLun },
    { USB_MASS_STORAGE_RESET            , USB_reqMassStorageReset },

    { 0, NULL }  // request handler table must terminate with a NULL member
};

/******************************************************************************/
/* Name     : USB_reqSetAddress                                               */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Set device address                                              */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqSetAddress(USB_REQUEST_ARGS)
{
    // set new device address sent in the wValue field of the setup packet
    USB_setDevAddr(DevNum, (Uint16)(USB_Setup->wValue));

    // request usb_ctl( ), usb control endpoint handler routine, to complete
    // the setup transaction with a 0-byte acknowledgement
    return(USB_REQUEST_SEND_ACK);
}


/******************************************************************************/
/* Name     : USB_reqSetConfiguration                                         */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Set/clear active configuration of the USB device                */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

extern unsigned short *data;
extern USB_EpObj usbEpObjOut2;
extern Uint16 Endpt2Buff[];

USB_REQUEST_RET USB_reqSetConfiguration(USB_REQUEST_ARGS)
{
    USB_REQUEST_RET ret_stat;

    // single configuration supported only
    if((USB_Setup->wValue == 0) || (USB_Setup->wValue == 1))
    {
        usbCurConfigStat = USB_Setup->wValue;
        *data++ = 0x9999;
        USB_postTransaction(&usbEpObjOut2, 64, &Endpt2Buff, USB_IOFLAG_NONE);
        ret_stat      = USB_REQUEST_SEND_ACK;
    }

    else
    {
        // configuration not supported, request usb_ctl( ), the usb control
        // endpoint handler routine to stall the endpoint
        ret_stat = USB_REQUEST_STALL;
    }

    return(ret_stat);
}


/******************************************************************************/
/* Name     : USB_reqClearFeature                                             */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Clear standard device features                                  */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqClearFeature(USB_REQUEST_ARGS)
{
    USB_EpHandle hEPx;
    Uint16 Endpt;
    USB_REQUEST_RET ret_stat = USB_REQUEST_SEND_ACK;

    // decode the requested feature
    switch(USB_Setup->wValue)
    {
        case USB_FEATURE_ENDPOINT_STALL:
        {
            // retrieve the endpoint number
            Endpt = (USB_Setup->wIndex) & 0xFF;
            // retrieve the handle associated with the endpoint
            hEPx = USB_epNumToHandle(USB0, Endpt);
            // stall the endpoint
            USB_clearEndptStall(hEPx);
            break;
        }
        case USB_FEATURE_REMOTE_WAKEUP:
        {
            // disable remote wakeup
            USB_setRemoteWakeup(USB0, USB_FALSE);
            break;
        }
        default:
        {
            // Unsupported feature, request the usb control endpoint handler
            // (usb_ctl( )) to stall the endpoint
            ret_stat = USB_REQUEST_STALL;
            break;
        }
    }

    return(ret_stat);
}


/******************************************************************************/
/* Name     : USB_reqGetStatus                                                */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Handle standard device request GET_STATUS                       */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqGetStatus(USB_REQUEST_ARGS)
{
    USB_EpHandle hEPx;
    Uint16 Endpt;      // this is USB logical endpoint
    USB_REQUEST_RET ret_stat = USB_REQUEST_GET_ACK;

    switch(USB_Setup->bmRequestType - 0x80)
    {
        case 0:          // return Device Status
        {
            gen_purpose_buffer[1] = (((Uint16)USB_getRemoteWakeupStat(USB0))<<1) | usbCurDevStat;
            USB_postTransaction(hInEp, 2, &gen_purpose_buffer, USB_IOFLAG_NONE);
            break;
        }

        case 1:          // return Interface status
        {
            gen_purpose_buffer[1] = usbCurIntrfcStat;
            USB_postTransaction(hInEp, 2, &gen_purpose_buffer, USB_IOFLAG_NONE);
            break;
        }

        case 2:           // return Endpoint status
        {
            Endpt                 = (USB_Setup->wIndex) & 0xFF;
            hEPx                  =  USB_epNumToHandle(USB0, Endpt);
            gen_purpose_buffer[1] = (Uint16)USB_getEndptStall(hEPx);
            USB_postTransaction(hInEp, 2, &gen_purpose_buffer, USB_IOFLAG_NONE);
            break;
        }

        default:
        {
            // Unsupported feature, request the usb control endpoint handler
            // (usb_ctl( )) to stall the endpoint
            ret_stat = USB_REQUEST_STALL;
            break;
        }
    }

    return(ret_stat);
}


/******************************************************************************/
/* Name     : USB_reqSetFeature                                                */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Handle standard device request GET_STATUS                       */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqSetFeature(USB_REQUEST_ARGS)
{
    USB_EpHandle hEPx;
    Uint16 Endpt;
    USB_REQUEST_RET ret_stat = USB_REQUEST_SEND_ACK;

    switch(USB_Setup->wValue)
    {
        case USB_FEATURE_ENDPOINT_STALL:    // stall endpoint
        {
            Endpt = (USB_Setup->wIndex) & 0xFF;
            hEPx = USB_epNumToHandle(USB0, Endpt);
            USB_stallEndpt(hEPx);
            break;
        }

        case USB_FEATURE_REMOTE_WAKEUP:    // enable remote wakeup
        {
            USB_setRemoteWakeup(USB0, USB_TRUE);
            break;
        }

        default:
        {
            // Feature not supported, request the usb control endpoint handler
            // (usb_ctl( )) to stall the endpoint
            ret_stat = USB_REQUEST_STALL;
            break;
        }
    }

    return(ret_stat);
}

/******************************************************************************/
/* Name     : USB_reqGetConfiguration                                         */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Return current device configuration                             */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqGetConfiguration(USB_REQUEST_ARGS)
{
    // Send the current Configuration Value
    gen_purpose_buffer[1] = usbCurConfigStat;

    USB_postTransaction(hInEp, 1, (void *)&gen_purpose_buffer,
                        USB_IOFLAG_NONE|USB_IOFLAG_NOSHORT);

    return(USB_REQUEST_GET_ACK);
}


/******************************************************************************/
/* Name     : USB_reqGetDescriptor                                            */
/*                                                                            */
/* Catagory : Standard Request Handler as defined in Chap 9 of USB spec 1.1   */
/*                                                                            */
/* Purpose  : Return requested descriptor                                     */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqGetDescriptor(USB_REQUEST_ARGS)
{
    Uint16 ii;
    Uint16 inout_flag;

    switch(USB_Setup->wValue>>8)
    {
        case USB_DESCRIPTOR_DEVICE:         // send device descriptor

        ii = device_descriptor[1]&0xFF;

        {
            // select the smaller of two
            ii = (ii < USB_Setup->wLength) ? ii : USB_Setup->wLength;
            USB_postTransaction(hInEp, ii, (void *)&device_descriptor[0], USB_IOFLAG_NONE);
        }
        break;

        case USB_DESCRIPTOR_CONFIG: // send config descriptor

            // config descriptor is made of linked lists
            inout_flag = (USB_IOFLAG_LNK | USB_IOFLAG_CAT);
            ii = configuration_descriptor[2];
            if(ii == USB_Setup->wLength) inout_flag |= USB_IOFLAG_NOSHORT;

            // select the smaller of two
            ii = (ii < USB_Setup->wLength) ? ii : USB_Setup->wLength;
            USB_postTransaction(hInEp, ii, &configuration_descriptor_link, inout_flag);
            break;

        case USB_DESCRIPTOR_STRING:         // send string descriptor

            if((USB_Setup->wValue & 0xFF) == 0) // LANGID Language Codes
            {
                ii = string_descriptor_langid[1] & 0xFF;
                // select the smaller of the two
                ii = (ii < USB_Setup->wLength) ? ii : USB_Setup->wLength;
                USB_postTransaction(hInEp, ii, &string_descriptor_langid[0], USB_IOFLAG_NONE);
            }
            else
            {
                // strlen of the string requested
                ii = (strlen(string_descriptor[USB_Setup->wValue & 0xFF])*2)-2 ;
                // select the smaller of the two
                ii = (ii < USB_Setup->wLength) ? ii : USB_Setup->wLength;

                // Insert descriptor type and length in first two bytes of string dscr
                string_descriptor[USB_Setup->wValue&0xFF][1] = (USB_DESCRIPTOR_STRING<<8) | ii;
                USB_postTransaction(hInEp, ii, &string_descriptor[USB_Setup->wValue&0xFF][0], USB_IOFLAG_NONE);
            }
            break;

        default: return(USB_REQUEST_STALL);
    }

    return(USB_REQUEST_GET_ACK);
}

/******************************************************************************/
/* Name     : USB_reqUnknown                                                  */
/*                                                                            */
/* Catagory :                                                                 */
/*                                                                            */
/* Purpose  : Respond to unknown requests                                     */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Comment  : USB_REQUEST_RET and USB_REQUEST_ARGS are defined in usb_req.h   */
/*            header file                                                     */
/*============================================================================*/

USB_REQUEST_RET USB_reqUnknown(USB_REQUEST_ARGS)
{
    USB_REQUEST_RET ret_stat = USB_REQUEST_DONE;

    if(USB_Setup->New)
    {
        // The request is either not known or not supported.
        // Request the usb control endpoint handler (usb_ctl( )) to stall the endpoint
        ret_stat = USB_REQUEST_STALL;
    }

    return(ret_stat);
}


/******************************************************************************/
/*                                                                            */
/*              Add user definded USB request handler below                   */
/*                                                                            */
/******************************************************************************/

