/**
 *  @file           main.c
 *
 *  @author         Leon Farchau and Damian Goldbach
 *  @date           20.05.2022

 */

// --- I N C L U D E S ---------------------------------------------------------
// STD
#include    <stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include    "stdint.h"

// CSL
#include    <csl.h>
#include    <csl_irq.h>
#include    <csl_mcbsp.h>
#include    <csl_timer.h>
#include    <csl_i2c.h>

// Modules
#include "util.h"
#include "aic23.h"
#include "main.h"
#include "usb/usbmain.h"
#include "audio_creation.h"


// --- D E F I N I T I O N S -------------------------------------------------


// --- G L O B A L   D A T A   D E F I N I T I O N S -------------------------------------------------


// Configurations ( cfg_ )

/** config for McBSP (multi-channel buffered serial port)   */
MCBSP_Config cfg_McBSP = {
    0x0007u,                    /*< Serial port control register 1 (SPCR1) */
    0x03C3u,                    /*< Serial port control register 2 (SPCR2) */
    0x00A0u,                    /*< Receive control register 1 (RCR1) */
    MCBSP_RCR2_DEFAULT,         /*< Receive control register 2 (RCR2) */
    0x00A0u,                    /*< Transmit control register 1 (XCR1) */
    MCBSP_XCR2_DEFAULT,         /*< Transmit control register 2 (XCR2) */
    0x0000u,                    /*< Sample rate generator register 1 */
    0x0000u,                    /*< Sample rate generator register 2 */
    MCBSP_MCR1_DEFAULT,         /*< Multi-channel control register 1 (MCR1) */
    MCBSP_MCR2_DEFAULT,         /*< Multi-channel control register 2 (MCR2) */
    0x0003u,                    /*< Pin control register (PCR) */
    MCBSP_RCERA_DEFAULT,        /*< Receive channel enable register partition A */
    MCBSP_RCERB_DEFAULT,        /*< Receive channel enable register partition B */
    MCBSP_RCERC_DEFAULT,        /*< Receive channel enable register partition C */
    MCBSP_RCERD_DEFAULT,        /*< Receive channel enable register partition D */
    MCBSP_RCERE_DEFAULT,        /*< Receive channel enable register partition E */
    MCBSP_RCERF_DEFAULT,        /*< Receive channel enable register partition F */
    MCBSP_RCERG_DEFAULT,        /*< Receive channel enable register partition G */
    MCBSP_RCERH_DEFAULT,        /*< Receive channel enable register partition H */
    MCBSP_XCERA_DEFAULT,        /*< Transmit channel enable register partition A */
    MCBSP_XCERB_DEFAULT,        /*< Transmit channel enable register partition B */
    MCBSP_XCERC_DEFAULT,        /*< Transmit channel enable register partition C */
    MCBSP_XCERD_DEFAULT,        /*< Transmit channel enable register partition D */
    MCBSP_XCERE_DEFAULT,        /*< Transmit channel enable register partition E */
    MCBSP_XCERF_DEFAULT,        /*< Transmit channel enable register partition F */
    MCBSP_XCERG_DEFAULT,        /*< Transmit channel enable register partition G */
    MCBSP_XCERH_DEFAULT         /*< Transmit channel enable register partition H */
};


// csl-config
#define TIMER_CTRL    TIMER_TCR_RMK(\
                      TIMER_TCR_IDLEEN_DEFAULT,    /* IDLEEN == 0 */ \
                      TIMER_TCR_FUNC_OF(0),        /* FUNC   == 0 */ \
                      TIMER_TCR_TLB_RESET,         /* TLB    == 1 */ \
                      TIMER_TCR_SOFT_BRKPTNOW,     /* SOFT   == 0 */ \
                      TIMER_TCR_FREE_WITHSOFT,     /* FREE   == 0 */ \
                      TIMER_TCR_PWID_OF(0),        /* PWID   == 0 */ \
                      TIMER_TCR_ARB_RESET,         /* ARB    == 1 */ \
                      TIMER_TCR_TSS_START,         /* TSS    == 0 */ \
                      TIMER_TCR_CP_PULSE,          /* CP     == 0 */ \
                      TIMER_TCR_POLAR_LOW,         /* POLAR  == 0 */ \
                      TIMER_TCR_DATOUT_0           /* DATOUT == 0 */ \
)

/* Create a TIMER configuration structure that can be passed */
/* to TIMER_config CSL function for initialization of Timer  */
/* control registers.                                        */
//Config for ??? Hz
TIMER_Config timCfg0 = {
   TIMER_CTRL,               /* TCR0 */
   13640,                  /* PRD0 */
   0x000F                    /* PRSC */
};


//Config for 48000 Hz
TIMER_Config timCfg1 = {
   TIMER_CTRL,               /* TCR0 */
   4000,                  /* PRD0 */
   0x000                    /* PRSC */
};

I2C_Setup cfg_I2C = {
    0,                          /*< Address Mode */
    0x0000,                     /*< Own Address */
    144,                        /*< System Clock Value (MHz) */
    100,                        /*< Desired Transfer rate ( 10 .. 400 kbps) */
    0,                          /*< Number of bits per byte to be received or transmitted */
    0,                          /*< Data Loopback mode */
    1                           /*< emulator FREE mode */
};

// --- G L O B A L   V A R I A B L E S ---------------------------------------

int volatile * const LEDs = (int *) 0x3F0000;
volatile int cnt_input = 0;



// ... H A N D L E R ( h_ ) ....................................................
MCBSP_Handle h_McBSP;
TIMER_Handle mhTimer0;
TIMER_Handle mhTimer1;

// ... E V E N T   I D ( eID_ ) ................................................
Uint16 eID_McBSP_rx;


Uint16 eventId0;
Uint16 eventId1;


// --- private function prototypes ---------------------------------
interrupt void timer0Isr(void);
void EnableAPLL( );
void wait( unsigned int cycles );


// *****************************************************************************
/**
 *  @fn             main( void )
 *  @brief          Hauptfunktion
 *  @details        Konfiguration der Peripherie und Initialisierung der
 *                  unterschiedlichen Programmteile vor dem ersten "Start" des
 *                  (eigentlichen) Programmablauf
 *
 *  @return         @b 0 - bei erfolgreicher Beendigung
 */
int main( void )
{
    // entrance   ( c_int00 ) .-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
   // void (*x)(void) = c_int00;

    EnableAPLL( );

    CSL_init();

    /* Set IVPH/IVPD to start of interrupt vector table */
    IRQ_setVecs(0x8000);

    /* Temporarily disable all maskable interrupts */
    IRQ_globalDisable();

    /* Open Timer 0, set registers to power on defaults */
    mhTimer0 = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);
    mhTimer1 = TIMER_open(TIMER_DEV1, TIMER_OPEN_RESET);

    /* Get Event Id associated with Timer 0, for use with */
    /* CSL interrupt enable functions.                    */
    eventId0 = TIMER_getEventId(mhTimer0);
    eventId1 = TIMER_getEventId(mhTimer1);

    /* Clear any pending Timer interrupts */
    IRQ_clear(eventId0);
    IRQ_clear(eventId1);

    /* Place interrupt service routine address at */
    /* associated vector location */
    IRQ_plug(eventId0,&timer0Isr);
    IRQ_plug(eventId1,&aud_sampleISR);

    /* Konfiguration des I2C Bus                                              */
    I2C_setup( &cfg_I2C );

    /* Konfiguration des Audio Codec via I2C                                  */
    aic23_reset();
    aic23_config(120);

    /* Konfiguration der McBSP Verbindung                                     */
    h_McBSP = MCBSP_open( MCBSP_PORT0, MCBSP_OPEN_RESET );
    eID_McBSP_rx = MCBSP_getRcvEventId( h_McBSP );
    MCBSP_config( h_McBSP, &cfg_McBSP );

    /* Write configuration structure values to Timer control regs */
    TIMER_config(mhTimer0, &timCfg0);
    TIMER_config(mhTimer1, &timCfg1);

    /* Enable Timer interrupt */
    IRQ_enable(eventId0);
    IRQ_enable(eventId1);

    /* Enable all maskable interrupts */
    IRQ_globalEnable();

    /* Start Timer */
    TIMER_start(mhTimer0);
    TIMER_start(mhTimer1);

    aud_init();
    USBTest_Init();


    while( 1 )
    {}

    // unreachable
    TIMER_close( mhTimer0 );
    MCBSP_close( h_McBSP );
    return 0;
}

// --- F U N K T I O N E N -----------------------------------------------------



void wait( unsigned int cycles )
{
    int i;
    for ( i = 0 ; i < cycles ; i++ ){ }
}

void EnableAPLL( )
{
    /* Enusre DPLL is running */
    *( ioport volatile unsigned short* )0x1f00 = 4;

    wait( 25 );

    *( ioport volatile unsigned short* )0x1f00 = 0;

    // MULITPLY
    *( ioport volatile unsigned short* )0x1f00 = 0x3000;

    // COUNT
    *( ioport volatile unsigned short* )0x1f00 |= 0x4F8;

    wait( 25 );

    //*( ioport volatile unsigned short* )0x1f00 |= 0x800

    // MODE
    *( ioport volatile unsigned short* )0x1f00 |= 2;

    wait( 30000 );

    // APLL Select
    *( ioport volatile unsigned short* )0x1e80 = 1;

    // DELAY
    wait( 60000 );
}

interrupt void timer0Isr(void)
{

}


