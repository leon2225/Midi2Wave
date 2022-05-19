/******************************************************************************/
/* Copyright (C) 2001  Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/*  File Name   : usb_ctrl.c                                                  */
/*  Project     : TMS320VC5509 USB Module Support                             */
/*  Author      : C5000 H/W Applications                                      */
/*                                                                            */
/*  Version     : 0.1                                                         */
/*  Date        : 30 Apr 2001                                                 */
/*  Updated     :                                                             */
/*                                                                            */
/*  Description : Example Control Endpoint (Endpoint 0) Event handler         */
/*                                                                            */
/******************************************************************************/
/*
 *  USB Test Component Copyright (C) 2004, Spectrum Digital, Inc.  All Rights Reserved.
 */

#include "csl.h"
#include "csl_usb.h"
#include "usb_req.h"
#include "usb_ctrl.h"

/******************************************************************************/
/*                   External Variables and Structures                        */
/*                                                                            */
/******************************************************************************/

// usbCurConfigStat defined in usb_req.c,  set usbCurConfigStat = 0 at reset
extern Uint16 usbCurConfigStat;

// USB_ReqTable[] is defined in the usb_req.c file.  USB_ctl( ) parses thru the
// USB_ReqTable[] to select the appropriate routine to service the usb
// request (from host)
extern USB_request_struct USB_ReqTable[];

// myUsbConfig[] array is defined in usb_main.c and used by the USB_ctl( ) to
// to reconfigure the USB module if host requests a reset.
extern USB_EpHandle  myUsbConfig[];



/******************************************************************************/
/*                   Control Endpoint Objects                                 */
/*                                                                            */
/******************************************************************************/

USB_EpObj   usbEpObjIn0, usbEpObjOut0;


/******************************************************************************/
/*                 Data structure to hold the setup packet                    */
/*                                                                            */
/******************************************************************************/

USB_SetupStruct USB_Setup = {0, 0, 0, 0, 0, 0};


/******************************************************************************/
/*                Function pointer for USB request handlers                   */
/*                                                                            */
/******************************************************************************/

// initialize the function pointer to the usb request handler with
// USB_reqUnknown( ), USB_reqUnknown( ) is defined in usb_req.c file
USB_REQUEST_RET (*fpRequestHandler)(USB_REQUEST_ARGS) = USB_reqUnknown;

unsigned short int *data = (unsigned short *)0x8000;

/******************************************************************************/
/* Name     : void USB_ctl_handler()                                          */
/*                                                                            */
/* Purpose  : USB Endpoint0 event handler routine                             */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/*                                                                            */
/*============================================================================*/
/* Arguments:                                                                 */
/*                                                                            */
/* hEp0In   : Handle to the In Endpoint0 object                               */
/*                                                                            */
/* hEp0Out  : Handle to the Out Endpoint0 object                              */
/*                                                                            */
/* DevNum   : USB device number (USB_DevNum type )                            */
/*                                                                            */
/*============================================================================*/
/* Return Value:                                                              */
/*                                                                            */
/* None                                                                       */
/*                                                                            */
/*============================================================================*/
/* Comments:                                                                  */
/*                                                                            */
/* if any USB bus event or endpoint0 related event takes place the USB event  */
/* dispatcher calls this routine to handle the events                         */
/*                                                                            */
/*============================================================================*/


void USB_ctl_handler()
{
    // instead of calling USB_ctl( ) this routine can post a SWI of DSP/BIOS
    // and SWI will in turn call the USB-ctl( )
    USB_ctl(USB0, &usbEpObjIn0, &usbEpObjOut0);
}

/******************************************************************************/
/* Name     : USB_ctl                                                         */
/*                                                                            */
/* Purpose  : Respond to the  USB bus events and handle the usb setup packets */
/*                                                                            */
/* Author   :                                                                 */
/*                                                                            */
/* Based on :                                                                 */
/*                                                                            */
/*                                                                            */
/*============================================================================*/
/* Arguments:                                                                 */
/*                                                                            */
/* DevNum   : USB device number (USB_DevNum type )                            */
/*                                                                            */
/* hEp0In   : Initialized handle to Endpt0 IN object                          */
/*                                                                            */
/* hEp0Out  : Initialized handle to Endpt0 OUT object                         */
/*                                                                            */
/*============================================================================*/
/* Return Value:                                                              */
/*                                                                            */
/* None                                                                       */
/*                                                                            */
/*============================================================================*/
/* Comments:                                                                  */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

void USB_ctl(USB_DevNum DevNum, USB_EpHandle hEp0In,USB_EpHandle hEp0Out)
{
    Uint16  Request;
    USB_REQUEST_RET ReqHandlerRet = USB_REQUEST_DONE;
    USB_EVENT_MASK  USB_ctl_events;


    // find out the endpt0 event caused this funcion to be called
    // and respond to the events
    USB_ctl_events = (USB_getEvents(hEp0Out) | USB_getEvents(hEp0In));

    // if the USB reset request received, abort all endpoint activities
    // and reconfigure the USB module

    if(USB_ctl_events & USB_EVENT_RESET)        // USB RESET event received
    {
        USB_abortAllTransaction(DevNum);        // stop all data transfer activities
        usbCurConfigStat = 0x00;                // reset device config number
        USB_init(USB0, myUsbConfig, 0x80);      // reconfig the USB module
    }

    if (USB_ctl_events & USB_EVENT_SUSPEND)     // USB SUSPEND event received
    {
        USB_issueRemoteWakeup( USB0 );
    }


    // if the event is a setup packet received event then read the setup packet,
    // and lookup the USB_ReqTable[] for the appropriate request handler

    if((USB_ctl_events & USB_EVENT_SETUP) == USB_EVENT_SETUP)
    {
        // read the setup packet, if something wrong with setup packet then stall
        // the control endpints
        if(USB_getSetupPacket(DevNum, &USB_Setup) == USB_FALSE)
        {
            ReqHandlerRet = USB_REQUEST_STALL;
        }
        else
        {
            #if(0)
                *data++ = 0x1234;
                *data++ = USB_Setup.bmRequestType;
                *data++ = USB_Setup.bRequest;
                *data++ = USB_Setup.wValue;
                *data++ = USB_Setup.wIndex;
                *data++ = USB_Setup.wLength;
            #endif

            // lookup the USB request handler
            Request = ((USB_Setup.bmRequestType&0xC0)<<8)|USB_Setup.bRequest;
            fpRequestHandler = USB_lookupReqHandler(Request, USB_ReqTable);
        }

    }   // end of if setup event received

    // Call the USB request handler unless ReqHandlerRet is
    // already set to USB_REQUEST_STALL by code above
    if(ReqHandlerRet == USB_REQUEST_DONE)
         // call the request handler pointed by fpRequestHandler
         ReqHandlerRet = (*fpRequestHandler)(DevNum, &USB_Setup, hEp0In, hEp0Out);
    else
        while(1);

    // based on the return value from the called request handler routine
    // send ACK, stall endpoint, or do nothing.
    switch(ReqHandlerRet)
    {
        // the request handler routine successfully completed the task,
        // send a 0-byte ACK
        case USB_REQUEST_SEND_ACK :
            USB_postTransaction(hEp0In, 0, NULL, USB_IOFLAG_NONE);
            fpRequestHandler = USB_reqUnknown;

        // request handler is not done, but waiting for more data from host,
        // do not modify fpRequestHandler, so the same request handler will
        // be called by usb_ctl() when the data packet arrive.
        case USB_REQUEST_DATA_OUT :
            break;

        // the request handler routine successfully completed the task,
        // get a 0-byte ACK
        case USB_REQUEST_GET_ACK :
            USB_postTransaction(hEp0Out, 0, NULL, USB_IOFLAG_NONE);
            fpRequestHandler = USB_reqUnknown;

        // request handler is not done, but waiting for more sending more data,
        // to host, do not modify fpRequestHandler, so the same request handler will
        // be called by usb_ctl() when the current data packet moves out of the
        // endpoint buffer.
        case USB_REQUEST_DATA_IN :
            break;

        // the request handler does not know what to do with the setup packet,
        // stall the control endpoints
        case USB_REQUEST_STALL :
            USB_stallEndpt(hEp0Out);
            USB_stallEndpt(hEp0In);
            break;

        case USB_REQUEST_DONE :
            fpRequestHandler = USB_reqUnknown;
            break;

        default:
            break;
    }

    // clear the flags if a new setup packet is received
    if(USB_Setup.New)
    {
        USB_Setup.New = 0;
    }

}


/******************************************************************************/
/* Name     : USB_lookupReqHandler                                            */
/*                                                                            */
/* Purpose  : Parse through the array of USB request handler and pickup the   */
/*            address of that particular request handler                      */
/*                                                                            */
/* Author   : MH                                                              */
/*                                                                            */
/* Based on :                                                                 */
/*                                                                            */
/*                                                                            */
/*============================================================================*/
/* Arguments:                                                                 */
/*                                                                            */
/* Request       : Request value retrieved from the first 2 bytes of USB      */
/*                 setup packet                                               */
/*                                                                            */
/* *pUSB_ReqTable: A pointer to the USB request handler table                 */
/*                                                                            */
/*============================================================================*/
/* Return Value:                                                              */
/*                                                                            */
/* Returns a function pointer to the USB request handler, a NULL pointer if   */
/* the desired request handler does not exist in the request handler table    */
/*                                                                            */
/*============================================================================*/
/* Comments:                                                                  */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
fpUSB_REQ_HANDLER USB_lookupReqHandler(Uint16 Request, USB_request_struct *pUSB_ReqTable)
{
    Uint16 ii;

    // parse thru the end of request handler table
    for(ii=0; pUSB_ReqTable[ii].fpRequestHandler != NULL ; ii++)
    {
        // if request handler exists return a pointer to the request handler routine
        if(pUSB_ReqTable[ii].Request == Request)
        {
            return(pUSB_ReqTable[ii].fpRequestHandler);
        }
    }
    // if request handler does not exist return a pointer to the USB_reqUnknown
    // routine
    return(USB_reqUnknown);
}

