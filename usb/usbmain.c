/******************************************************************************/
/* Copyright (C) 2001  Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/*  File Name   : usbmain.c                                                   */
/*  Project     : TMS320VC5509 USB Module Support                             */
/*  Author      : MH                                                          */
/*                                                                            */
/*  Version     : 0.1                                                         */
/*                                                                            */
/*  Description : Main routine for 5509 USB demo application                  */
/*                                                                            */
/*  Created on  : 30 Apr 2001                                                 */
/*                                                                            */
/*  Updates     :                                                             */
/*                                                                            */
/*  17 May 2001 $MH$                                                          */
/*  Added isochronous and host dma features                                   */
/*                                                                            */
/******************************************************************************/
/*
 *  USB Test Component Copyright (C) 2004, Spectrum Digital, Inc.  All Rights Reserved.
 */

#include "csl.h"
#include "csl_usb.h"
#include "csl_irq.h"

#include "stdio.h"
#include "stdint.h"

#include "audio_creation.h"
#undef DEBUGUSB

/******************************************************************************/
/* Define Interrupt Flag and Interrupt Mask Registers                         */
/*                                                                            */
/******************************************************************************/
#define IER0_ADDR        0x0000
#define IER0    *(volatile unsigned int*)IER0_ADDR

#define IFR0_ADDR        0x0001
#define IFR0    *(volatile unsigned int*)IFR0_ADDR

#define IER1_ADDR        0x0045
#define IER1    *(volatile unsigned int*)IER1_ADDR

#define IFR1_ADDR        0x0046
#define IFR1    *(volatile unsigned int*)IFR1_ADDR

#define IVPD_ADDR        0x0049
#define IVPD    *(volatile unsigned int*)IVPD_ADDR

#define IVPH_ADDR        0x004a
#define IVPH    *(volatile unsigned int*)IVPH_ADDR

/******************************************************************************/
/*                          Symbolic Constants                                */
/*                                                                            */
/******************************************************************************/

#define IER0_USBMSK      0x0100   // mask for USB intrpt
#define IFR0_USBMSK      0x0100   // mask for USB intrpt


/******************************************************************************/
/* INTR_ENABLE - enables all masked interrupts by resetting INTM              */
/*               bit in Status Register 1                                     */
/******************************************************************************/
#define INTR_GLOBAL_ENABLE \
        asm("    bit(st1, #st1_intm) = #0");

/******************************************************************************/
/* INTR_DISABLE - disables all masked interrupts by setting INTM              */
/*                bit in Status Register 1                                    */
/******************************************************************************/
#define INTR_GLOBAL_DISABLE \
        asm("    bit(st1, #st1_intm) = #1");


/******************************************************************************/
/* CLOCK MODE REGISTER                                                        */
/* Defined for C55XX                                                          */
/******************************************************************************/
#define CLKMD_ADDR      0x1C00
#define CLKMD           *(ioport volatile unsigned int *)CLKMD_ADDR

/******************************************************************************/
/* Data bitfields for Clock Mode Register (CLKMD)                             */
/*                                                                            */
/******************************************************************************/
#define IAI                14
#define IOB                13
#define PLL_MULT           7
#define PLL_DIV            5
#define PLL_ENABLE         4
#define BYPASS_DIV         2
#define BREAKLN            1
#define LOCK               0

/******************************************************************************/
/*                   Prototype: DSP Clock Init Function                       */
/*                                                                            */
/******************************************************************************/
void dspclk_init(Uint16 inclk, Uint16 outclk, Uint16 plldiv);

/******************************************************************************/
/*               DSP Interrupt Vector Table base address                      */
/*                                                                            */
/******************************************************************************/
// extern void RESET_VEC();

/******************************************************************************/
/*                           Endpoint Objects                                 */
/*                                                                            */
/******************************************************************************/

extern USB_EpObj   usbEpObjOut0, usbEpObjIn0;   // Control Endpoint Objects
USB_EpObj          usbEpObjOut2, usbEpObjIn2;   // Bulk endpoint objects

int postcsw = 0;
int readinprogress = 0;
int writeinprogress = 0;

/******************************************************************************/
/*                   Prototype: Endpoint Event Handler                        */
/*                                                                            */
/******************************************************************************/
extern void USB_ctl_handler();       // Control Endpoint Event handler

void USB_bulkOutEvHandler();         // Endpoint2 OUT event handler
                                     // Endpoint2 OUT data buffer handler
void USB_bulkOutDatHandler(USB_EpHandle hEpIn, USB_EpHandle hEpOut);

void USB_bulkInEvHandler();          // Endpoint2 IN event handler
                                     // Endpoint2 IN data buffer handler
void USB_bulkInDatHandler(USB_EpHandle hEpIn, USB_EpHandle hEpOut);

/******************************************************************************/
/*                   Endpoint Data Buffers                                    */
/*                                                                            */
/******************************************************************************/


#define Endpt2BuffLen 0xFFFF
Uint16 Endpt2Buff[Endpt2BuffLen>>1];


/******************************************************************************/
/*                         USB Configuration                                  */
/*                                                                            */
/******************************************************************************/
USB_EpHandle  myUsbConfig[] = {&usbEpObjOut0, &usbEpObjIn0, &usbEpObjOut2,
                               &usbEpObjIn2, NULL};

/******************************************************************************/
/*                         main()                                             */
/*                                                                            */
/******************************************************************************/

unsigned short csw[9];
unsigned long lba;
unsigned short blocksleft;

//extern interrupt void _USB_isr();
interrupt void _USB_isr();
interrupt void _USB_isr()
{
    USB_evDispatch( );
}

unsigned short USBTest_Init()
{
    USB_Boolean  usb_status;
    Uint16       event_mask;
    Uint16       usbId = IRQ_EVT_USB;

    // initialize preliminary setup

    // if using DSP/BIOS, DSP clock will be setup by DSP/BIOS
    //  dspclk_init(12, 96, 0);       // init DSP clock

    //INT_InstallHandler(8, _USB_isr);
    //INT_InstallHandler(8, USB_evDispatch);

    // IMPORTANT:API vector address must be initialized before calling any USB API
    USB_setAPIVectorAddress();      // Enable USB module, initialize USB API
                                    // vector base address

    // initialize endpoint objects
    // endpoint object initialization is not necessary if CCS config tools used.
    // CCS config tools initilize the endpoint objects statically

    // init endpoint 0 objects
    // endpoint out0 will respond to following events.
    event_mask = USB_EVENT_RESET | USB_EVENT_SETUP | USB_EVENT_SUSPEND |
                 USB_EVENT_RESUME | USB_EVENT_EOT;

    usb_status = USB_initEndptObj(USB0, &usbEpObjOut0, USB_OUT_EP0, USB_CTRL,
                                0x40, event_mask, USB_ctl_handler);
    usb_status = USB_initEndptObj(USB0, &usbEpObjIn0, USB_IN_EP0, USB_CTRL,
                                0x40, USB_EVENT_EOT,USB_ctl_handler);

    // init endpoint 2 objects - bulk mode, endpoint size 64 byte
    // dispatch end of transfer event to endpoint object
    usb_status = USB_initEndptObj(USB0, &usbEpObjOut2, USB_OUT_EP2, USB_BULK,
                                0x40, USB_EVENT_EOT,USB_bulkOutEvHandler);
    // dispatch end of transfer and USB reset events to endpoint object
    usb_status = USB_initEndptObj(USB0, &usbEpObjIn2, USB_IN_EP2, USB_BULK, 0x40,
                                USB_EVENT_EOT|USB_EVENT_RESET,USB_bulkInEvHandler);

    // init endpoint3 object - interrupt mode, endpoint size 64 byte
    // dispatch end of transfer and USB reset events to endpoint object
    //  usb_status = USB_initEndptObj(USB0, &usbEpObjIn3, USB_IN_EP3, USB_INTR, 0x40,
    //                                USB_EVENT_EOT|USB_EVENT_RESET,USB_endpt3EvHandler);

    // init endpoint 4 objects - bulk mode (for host dma operation mode)
    // endpoint size 64 byte, no event to dispatch to the endpoint object
    //  usb_status = USB_initEndptObj(USB0, &usbEpObjOut4, USB_OUT_EP4, USB_BULK,
    //                                0x40, USB_EVENT_NONE, USB_EvISR_NONE);
    // no event to dispatch to the endpoint object
    //  usb_status = USB_initEndptObj(USB0, &usbEpObjIn4, USB_IN_EP4, USB_BULK, 0x40,
    //                                USB_EVENT_NONE, USB_EvISR_NONE);


    // init endpoint5 object - iso mode, endpoint size 16 byte
    // dispatch pre-start-of-frame events to endpoint object
    //  usb_status = USB_initEndptObj(USB0, &usbEpObjIn5, USB_IN_EP5, USB_ISO, 0x10,
    //                                USB_EVENT_PSOF, USB_isoInEvHandler);

    // init USB module - module will assume configuration defined by myUsbConfig[]
    // PSOF interrupt occurs 128 ticks (750kHz clock) prior to SOF.
    usb_status = USB_init(USB0, myUsbConfig, 0x80);

    /* Clear any pending interrupts for DMA channels */
    IRQ_clear(usbId);
    IRQ_enable(usbId);

    /* Place DMA interrupt service addresses at associate vector */
    IRQ_plug(usbId,&_USB_isr);

    /* Enable all maskable interrupts */
    IRQ_globalEnable();

    // enable USB interrupt:

    // if using DSP/BIOS, these regs will be initialized by DSP/BIOS
    //  IVPD  = ((Uint32)RESET_VEC>>8) & 0xFFFF; // init IVPD reg
    //  IVPH  = ((Uint32)RESET_VEC>>8) & 0xFFFF; // init IVPH reg
    //IFR0 |= IFR0_USBMSK;                    // Clear USB intrpt flag
    //IER0 |= IER0_USBMSK;                    // Enable USB intrpt mask
    //INTR_GLOBAL_ENABLE;                     // enable global interrupt

    // if the USB module configured properly,
    // connect it to up stream port

    if(usb_status) USB_connectDev(USB0);

    //===========================================================================
    //
    // Once the USB module is connected to the up stream port following
    // things happen:
    //
    // a.  The USB host contoller resets the USB module
    //
    // b.  The USB Event Dispatcher broadcast the RESET event to 0-OUT.
    //     2-IN, and 3-IN endpoints by calling the respective endpoint
    //     event handler routines in sequence.
    //
    // c.  In response to the RESET event the USB_ctl_handler() calls
    //     USB_ctl( ) which terminates all endpoint activities and
    //     reconfigures the USB module by call USB_setParams() API.
    //
    // d.  In response to the RESET event the USB_bulkInEvHandler()
    //     calls the USB_bulkInDatHandler( ) which posts a bulk
    //     transfer via 2-OUT endpoint.
    //
    // c.  In response to the RESET event the USB_endpt3EvHandler()
    //     calls the USB_endpt3DatHandler( ) which posts an interrupt
    //     transfer via 3-IN endpoint.
    //
    // e.  Host sends SETUP packets requesting descriptors for enumeration
    //     purpose.
    //
    // f.  The USB Event Dispatcher broadcast the SETUP event to the
    //     USB_ctl_handler().  The USB_ctl_handler() calls the USB_ctl()
    //     routine.  The USB_ctl() parses through the USB request handler
    //     table and calls the appropriate request handler routine to
    //     send the requested descriptor to the host.
    //
    // g.  Host sends multiple SETUP packets to complete the enumeration
    //     process.
    //
    // h.  Host loads the device driver(s) to communicate with USB module.
    //
    // i.  Host application sends some arbitrary number (max 64) of bytes
    //     of data to 2-OUT endpoint.
    //
    // j.  Data moves into the Endpt2Buff[258] and generates a EOT(end of
    //     transfer) event for 2-OUT endpoint.
    //
    // k.  USB Event Dispatcher boardcast the EOT event to USB_bulkOutEvHandler()
    //     which in turn calls USB_bulkOutDatHandler(..).
    //
    // l.  The USB_bulkOutDatHandler(..) loops the received data back to
    //     the host via 2-IN endpoint.
    //
    // m.  Host reads the data which generates an EOT event for 2-IN endpoint.
    //
    // n.  USB Event Dispatcher boardcast the EOT event to USB_bulkInEvHandler()
    //     which in turn calls USB_bulkInDatHandler(..).
    //
    // o.  The USB_bulkInDatHandler(..) posts a bulk transfer via 2-OUT endpoint.
    //
    // p.  Step i through o continues as long the host application keeps on
    //     sending and receiving data via 2-OUT and 2-IN endpoint respectively.
    //
    // q.  For the interrupt endpoint the host application sends an IN token.
    //
    // r.  Data moves out of the 3-IN endpoint and generates an EOT event.
    //
    // s.  USB Event Dispatcher boardcast the EOT event to USB_endpt3EvHandler()
    //     which in turn calls USB_endpt3DatHandler(..).
    //
    // t.  The USB_endpt3DatHandler(..)posts the next interrupt transfer via
    //     3-IN endpoint.
    //
    // p.  Step q through t continues as long the host application keeps on
    //     reading data from 3-IN endpoint.
    //
    //===========================================================================


    // Post first transaction to get things started
    USB_postTransaction(&usbEpObjOut2, 0, 0, USB_IOFLAG_NONE);
    //USB_postTransaction(&usbEpObjIn2, 0, 0, USB_IOFLAG_NONE);

    // go to idle loop, since all the USB activities are interrupt driven
    // while(1)    /* loop idle */
    // {
    //     for(ii=0; ii<30000; ii++);
    // }

    return 0;
}   // end of main

/******************************************************************************/
/*                     Bulk Out Event Handlers                                */
/*                                                                            */
/******************************************************************************/
void USB_bulkOutEvHandler()
{
    // call the bulk out data handler routine.

    // if DSP/BIOS used, a SWI can be posted to service the endpoint events.
    // All USB event handler routines must be executed in the order the
    // actual events arrived - to achieve this, if SWI used, all the USB
    // event handler routines must be assigned the same priority level.

    USB_bulkOutDatHandler(&usbEpObjIn2, &usbEpObjOut2);
}

void USB_bulkOutDatHandler(USB_EpHandle hEpIn, USB_EpHandle hEpOut)
{
    if ( USB_isTransactionDone( hEpOut ) )
    {
        //puts("Received Data");
        //Uint32 in = ((Uint32)*(Endpt2Buff+2) << 16) | (*(Endpt2Buff+1));
        int index = 0;
        for (index = 0; index < CHANNELS; ++index) {
            aud_setTone(*(Endpt2Buff+1+index),index);
        }
        /*char inArray[4];
        int i = 0;
        for(i = 0; i<4; i++)
        {
            inArray[i] = (((char*) (Endpt2Buff + 1))[i] &0xFF) - '0' ;
        }*/



        USB_postTransaction(hEpOut, Endpt2BuffLen, &Endpt2Buff, USB_IOFLAG_NONE );
    }

}

/******************************************************************************/
/*                      Bulk In Event Handlers                                */
/*                                                                            */
/******************************************************************************/

void USB_bulkInEvHandler()
{
    // call the bulk in data handler routine.

    // if DSP/BIOS used, a SWI can be posted to service the endpoint events.
    // All USB event handler routines must be executed in the order the
    // actual events arrived - to achieve this, if SWI used, all the USB
    // event handler routines must be assigned the same priority level.

    USB_bulkInDatHandler(&usbEpObjIn2, &usbEpObjOut2);
}

void USB_bulkInDatHandler(USB_EpHandle hEpIn, USB_EpHandle hEpOut)
{
    if ( USB_isTransactionDone( hEpIn ) )
        USB_postTransaction(&usbEpObjIn2, Endpt2BuffLen, Endpt2Buff, USB_IOFLAG_NONE );
}
