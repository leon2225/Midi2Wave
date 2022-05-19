/******************************************************************************/
/* $Id: usb_ctrl.h, 25 Jan 01 $MH$                                            */
/*                                                                            */
/* Copyright (C) 2001, Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/* Header file for usb_ctrl.c                                                 */
/*                                                                            */
/******************************************************************************/

#ifndef _USB_CTRL_H
#define _USB_CTRL_H

/******************************************************************************/
/*                                                                            */
/*       USB                  ??               */
/*                                                                            */
/******************************************************************************/

typedef  USB_REQUEST_RET (*fpUSB_REQ_HANDLER)(USB_REQUEST_ARGS);

fpUSB_REQ_HANDLER USB_lookupReqHandler(Uint16 Request, USB_request_struct *pUSB_ReqTable);
void USB_ctl(USB_DevNum DevNum, USB_EpHandle hEp0In,USB_EpHandle hEp0Out);

#endif /* _USB_CTRL55XX_H */
