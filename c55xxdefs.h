/*H***************************************************************************
*
* $Archive::                                                                $
* $Revision::                                                               $
* $Date::                                                                   $
* $Author::                                                                 $
*
* DESCRIPTION:
*
* USAGE/LIMITATIONS:
*
* NOTES:
*
* (C) Copyright 1997 by Spectrum Digital Incorporated
* All rights reserved
*
*H***************************************************************************/

#ifndef c55xxdefs_h
#define c55xxdefs_h

/*---- compilation control switches ----------------------------------------*/

/*****************************************************************************
* INCLUDE FILES (minimize nesting of header files)
*****************************************************************************/

/*---- system and platform files -------------------------------------------*/

/*---- program files -------------------------------------------------------*/

/*****************************************************************************
* FILE CONTENT
*****************************************************************************/

/*****************************************************************************
* FUNCTIONAL AREA DETAIL
*****************************************************************************/

/*A***************************************************************************
* NAME:
*
* USAGE:
*
* NOTES:
*
*A***************************************************************************/

/*---- context -------------------------------------------------------------*/

/*---- data descriptions ---------------------------------------------------*/
typedef ioport volatile unsigned short * PIOPUS;  /* Generic pointer to io  */

/*---- Memory mapped CPU regisers-------------------------------------------*/
typedef struct c55xx_mmr
{
    unsigned short ier0;       /*  0 */
    unsigned short ifr0;       /*  1 */
    unsigned short st0_55;     /*  2 */
    unsigned short st1_55;     /*  3 */
    unsigned short st3_55;     /*  4 */
    unsigned short rsv0;       /*  5 */
    unsigned short st0;        /*  6 */
    unsigned short st1;        /*  7 */
    unsigned short ac0l;       /*  8 */
    unsigned short ac0h;       /*  9 */
    unsigned short ac0g;       /*  A */
    unsigned short ac1l;       /*  B */
    unsigned short ac1h;       /*  C */
    unsigned short ac1g;       /*  D */
    unsigned short t3;         /*  E */
    unsigned short trn0;       /*  F */
    unsigned short ar0;        /* 10 */
    unsigned short ar1;        /* 11 */
    unsigned short ar2;        /* 12 */
    unsigned short ar3;        /* 13 */
    unsigned short ar4;        /* 14 */
    unsigned short ar5;        /* 15 */
    unsigned short ar6;        /* 16 */
    unsigned short ar7;        /* 17 */
    unsigned short sp;         /* 18 */
    unsigned short bk03;       /* 19 */
    unsigned short brc0;       /* 1A */
    unsigned short rsa0l;      /* 1B */
    unsigned short rea0l;      /* 1C */
    unsigned short pmst;       /* 1D */
    unsigned short xpc;        /* 1E */
    unsigned short rsv1;       /* 1F */
    unsigned short _t0;        /* 20 */
    unsigned short _t1;        /* 21 */
    unsigned short _t2;        /* 22 */
    unsigned short _t3a;       /* 23 */
    unsigned short _ac2l;      /* 24 */
    unsigned short _ac2h;      /* 25 */
    unsigned short _ac2g;      /* 26 */
    unsigned short _cdp;       /* 27 */
    unsigned short _ac3l;      /* 28 */
    unsigned short _ac3h;      /* 29 */
    unsigned short _ac3g;      /* 2A */
    unsigned short _dph;       /* 2B */
    unsigned short _mdp05;     /* 2C */
    unsigned short _mdp67;     /* 2D */
    unsigned short _dp;        /* 2E */
    unsigned short _pdp;       /* 2F */
    unsigned short _bk47;      /* 30 */
    unsigned short _bkc;       /* 31 */
    unsigned short _bsa01;     /* 32 */
    unsigned short _bsa23;     /* 33 */
    unsigned short _bsa45;     /* 34 */
    unsigned short _bsa67;     /* 35 */
    unsigned short _bsac;      /* 36 */
    unsigned short _bios;      /* 37 */
    unsigned short _trn1;      /* 38 */
    unsigned short _brc1;      /* 39 */
    unsigned short _brs1;      /* 3A */
    unsigned short _csr;       /* 3B */
    unsigned short _rsa0h;     /* 3C */
    unsigned short _rsa0l;     /* 3D */
    unsigned short _rea0h;     /* 3E */
    unsigned short _rea0l;     /* 3F */
    unsigned short _rsa1h;     /* 40 */
    unsigned short _rsa1l;     /* 41 */
    unsigned short _rea1h;     /* 42 */
    unsigned short _rea1l;     /* 43 */
    unsigned short _rptc;      /* 44 */
    unsigned short _ier1;      /* 45 */
    unsigned short _ifr1;      /* 46 */
    unsigned short _dbier0;    /* 47 */
    unsigned short _dbier1;    /* 48 */
    unsigned short _ivpd;      /* 49 */
    unsigned short _ivph;      /* 4A */
    unsigned short _st2_55;    /* 4B */
    unsigned short _ssp;       /* 4C */
    unsigned short _sp;        /* 4D */
    unsigned short _sph;       /* 4E */
    unsigned short _cdph;      /* 4F */
} C55XX_MMR;

typedef  volatile C55XX_MMR * PC55XX_MMR;
#define C55XX_MMR_ADDR         ((unsigned long)0) /* MMR in data space not i/0        */
/*---- External memory interface -------------------------------------------*/
typedef struct c55xx_rhea
{
    unsigned short cmr;         /* 00 */
    unsigned short icr;         /* 01 */
    unsigned short istr;        /* 02 */
}C55XX_RHEA;

typedef ioport volatile C55XX_RHEA * PC55XX_RHEA;

/*---- External memory interface -------------------------------------------*/
typedef struct c55xx_emif
{
    unsigned short egcr;        /* 00 */
    unsigned short rst;         /* 01 */
    unsigned short be;          /* 02 */
    unsigned short ce0_1;       /* 03 */
    unsigned short ce0_2;       /* 04 */
    unsigned short ce0_3;       /* 05 */
    unsigned short ce1_1;       /* 06 */
    unsigned short ce1_2;       /* 07 */
    unsigned short ce1_3;       /* 08 */
    unsigned short ce2_1;       /* 09 */
    unsigned short ce2_2;       /* 0A */
    unsigned short ce2_3;       /* 0B */
    unsigned short ce3_1;       /* 0C */
    unsigned short ce3_2;       /* 0D */
    unsigned short ce3_3;       /* 0E */
    unsigned short sdc1;        /* 0F */
    unsigned short sdper;       /* 10 */
    unsigned short sdcnt;       /* 11 */
    unsigned short sdinit;      /* 12 */
    unsigned short sdc2;        /* 13 */


}C55XX_EMIF;

typedef ioport volatile C55XX_EMIF * PC55XX_EMIF;

/*---- Multi-channel serial port -------------------------------------------*/
typedef struct c55xx_mcsp
{
    unsigned short ddr2;        /*00 */
    unsigned short ddr1;        /*01 */
    unsigned short dxr2;        /*02 */
    unsigned short dxr1;        /*03 */
    unsigned short spcr2;       /*04 */
    unsigned short spcr1;       /*05 */
    unsigned short rcr2;        /*06 */
    unsigned short rcr1;        /*07 */
    unsigned short xcr2;        /*08 */
    unsigned short xcr1;        /*09 */
    unsigned short srgr2;       /*0A */
    unsigned short srgr1;       /*0B */
    unsigned short mcr2;        /*0C */
    unsigned short mcr1;        /*0D */
    unsigned short rcera;       /*0E */
    unsigned short rcerb;       /*0F */
    unsigned short xcera;       /*10 */
    unsigned short xcerb;       /*11 */
    unsigned short pcr;         /*12 */
    unsigned short pcerc;       /*13 */
    unsigned short pcerd;       /*14 */
    unsigned short xcerc;       /*15 */
    unsigned short xcerd;       /*16 */
    unsigned short rcere;       /*17 */
    unsigned short rcerf;       /*18 */
    unsigned short xcere;       /*19 */
    unsigned short xcerf;       /*1A */
    unsigned short rcerg;       /*1B */
    unsigned short rcerh;       /*1C */
    unsigned short xcerg;       /*1D */
    unsigned short xcerh;       /*1E */
}C55XX_MCSP;

typedef ioport volatile C55XX_MCSP * PC55XX_MCSP;

/*---- Timer ---------------------------------------------------------------*/
typedef struct c55xx_timer
{
    unsigned short tim;         /* Timer register                           */
    unsigned short prd;         /* Timer period register                    */
    unsigned short tcr;         /* Timer control register                   */
    unsigned short prsc;        /* Timer prescaler register                 */
}C55XX_TIMER;

typedef ioport volatile C55XX_TIMER * PC55XX_TIMER;

/*---- RTC -----------------------------------------------------------------*/
typedef struct c55xx_rtc
{
    unsigned short rtcsec;      /* 00 */
    unsigned short rtcseca;     /* 01 */
    unsigned short rtcmin;      /* 02 */
    unsigned short rtcmina;     /* 03 */
    unsigned short rtchour;     /* 04 */
    unsigned short rtchoura;    /* 05 */
    unsigned short rtcdayw;     /* 06 */
    unsigned short rtcdaym;     /* 07 */
    unsigned short rtcmonth;    /* 08 */
    unsigned short rtcyear;     /* 09 */
    unsigned short rtcpintr;    /* 0A */
    unsigned short rtcinten;    /* 0B */
    unsigned short rtcintfl;    /* 0C */
}C55XX_RTC;

typedef ioport volatile C55XX_RTC * PC55XX_RTC;


/*---- DMA channel ---------------------------------------------------------*/
typedef struct c55xx_dma
{
    unsigned short csdp;        /* 00 */
    unsigned short ccr;         /* 01 */
    unsigned short cicr;        /* 02 */
    unsigned short csr;         /* 03 */
    unsigned short cssa_l;      /* 04 */
    unsigned short cssa_u;      /* 05 */
    unsigned short cdsa_l;      /* 06 */
    unsigned short cdsa_u;      /* 07 */
    unsigned short cen;         /* 08 */
    unsigned short cfn;         /* 09 */
    unsigned short cfi;         /* 0A */
    unsigned short cei;         /* 0B */
}C55XX_DMA;

typedef ioport volatile C55XX_DMA * PC55XX_DMA;


/*---- Trace FIFO ----------------------------------------------------------*/
typedef struct c55xx_trace
{
    unsigned short trc00_63[0x40];  /* 00 */
    unsigned short trc63_79[0x10];  /* 40 */
    unsigned short lpoffset1;       /* 50 */
    unsigned short lpoffset2;       /* 51 */
    unsigned short ptr;             /* 52 */
    unsigned short cntl;            /* 53 */
    unsigned short id;              /* 54 */
}C55XX_TRACE;

typedef ioport volatile C55XX_TRACE * PC55XX_TRACE;


/*---- Clock Mode ----------------------------------------------------------*/
typedef struct c55xx_cmod
{
    unsigned short clkmd;       /* Clock-mode register                      */

}C55XX_CMOD;

typedef ioport volatile  C55XX_CMOD * PC55XX_CMOD;

/*---- Clock Mode ----------------------------------------------------------*/
typedef struct c55xx_usbcmod
{
    unsigned short usbclkmd;       /* Clock-mode register                      */

}C55XX_USBCMOD;

typedef ioport volatile  C55XX_USBCMOD * PC55XX_USBCMOD;


/*---- GPIO  ---------------------------------------------------------------*/
typedef struct c55xx_gpio
{
    unsigned short iodir;           /* 00 */
    unsigned short iodata;          /* 01 */
}C55XX_GPIO;

typedef ioport volatile C55XX_GPIO * PC55XX_GPIO;

/*---- Die Id  --------------------------------------------------------------*/
typedef struct c55xx_did
{
    unsigned short id0;             /* 00 */
    unsigned short id1;             /* 01 */
    unsigned short id2;             /* 02 */
    unsigned short id3;             /* 03 */
}C55XX_DID;

typedef ioport volatile C55XX_DID *PC55XX_DID;

/*---- I2C ------------------------------------------------------------------*/
typedef struct c55xx_i2c
{
    unsigned short icoar;           /* 00 */
    unsigned short icimr;           /* 01 */
    unsigned short icstr;           /* 02 */
    unsigned short icclkl;          /* 03 */
    unsigned short icclkh;          /* 04 */
    unsigned short iccnt;           /* 05 */
    unsigned short icdrr;           /* 06 */
    unsigned short icsar;           /* 07 */
    unsigned short icdxr;           /* 08 */
    unsigned short icmdr;           /* 09 */
    unsigned short icivr;           /* 0A */
    unsigned short icgpio;          /* 0B */
    unsigned short icpsc;           /* 0C */
}C55XX_I2C;

typedef ioport volatile C55XX_I2C *PC55XX_I2C;

/*---- Media Card------------------------------------------------------------*/
typedef struct c55xx_mmc
{
    unsigned short fckctl;          /* 00 */
    unsigned short ctl;             /* 01 */
    unsigned short clk;             /* 02 */
    unsigned short st0;             /* 03 */
    unsigned short st1;             /* 04 */
    unsigned short im;              /* 05 */
    unsigned short tor;             /* 06 */
    unsigned short tod;             /* 07 */
    unsigned short blen;            /* 08 */
    unsigned short nblk;            /* 09 */
    unsigned short nblc;            /* 0A */
    unsigned short ddr;             /* 0B */
    unsigned short dxr;             /* 0C */
    unsigned short cmd;             /* 0D */
    unsigned short argl;            /* 0E */
    unsigned short argh;            /* 0F */
    unsigned short rsp0;            /* 10 */
    unsigned short rsp1;            /* 11 */
    unsigned short rsp2;            /* 12 */
    unsigned short rsp3;            /* 13 */
    unsigned short rsp4;            /* 14 */
    unsigned short rsp5;            /* 15 */
    unsigned short rsp6;            /* 16 */
    unsigned short rsp7;            /* 17 */
    unsigned short drsp;            /* 18 */
    unsigned short etok;            /* 19 */
    unsigned short cidx;            /* 1A */
}C55XX_MMC;

#ifdef MMC_EXTERNAL
/*-- Data pointer if external, io pointer if on-chip ----------------------*/
typedef  volatile C55XX_MMC * PC55XX_MMC;
#else
typedef ioport volatile C55XX_MMC * PC55XX_MMC;
#endif

/*---- Memory Stick------------------------------------------------------------*/
typedef struct c55xx_ms
{
    unsigned short cmd;             /* 00 */
    unsigned short stat;            /* 01 */
    unsigned short cntl;            /* 02 */
    unsigned short idata;           /* 03 */
    unsigned short ictl;            /* 04 */
    unsigned short ppctl;           /* 05 */
    unsigned short drr;             /* 06 */
    unsigned short dxr;             /* 07 */
    unsigned short fclk;            /* 08 */
    unsigned short clk;             /* 09 */
    unsigned short nu1;             /* 0A */
    unsigned short nu2;             /* 0B */
    unsigned short nu3;             /* 0C */
    unsigned short nu4;             /* 0D */
    unsigned short nu5;             /* 0E */
    unsigned short nu6;             /* 0F */
    unsigned short nu7;             /* 10 */
    unsigned short nu8;             /* 11 */
    unsigned short nu9;             /* 12 */
    unsigned short nu10;            /* 13 */
    unsigned short ppdata;          /* 14 */
}C55XX_MS;

#ifdef MMC_EXTERNAL
/*-- Data pointer if external, io pointer if on-chip ----------------------*/
typedef volatile C55XX_MS * PC55XX_MS;
#else
typedef ioport volatile C55XX_MS * PC55XX_MS;
#endif

/*---- Cpu Revision---------------------------------------------------------*/
typedef struct c55xx_revid
{
    unsigned short revid;           /* 00 */
}C55XX_REVID;

typedef ioport volatile C55XX_REVID *PC55XX_REVID;

/*---- Interrupt table definition ------------------------------------------*/
typedef struct c55xx_intr
{
    unsigned short ins1;       /* Instruction slot 1                        */
    unsigned short ins2;       /* Instruction slot 2                        */
    unsigned short ins3;       /* Instruction slot 3                        */
    unsigned short ins4;       /* Instruction slot 4                        */
}C55XX_INTR;

typedef volatile C55XX_INTR * PC55XX_INTR;

typedef struct c55xx_intr_table
{
    C55XX_INTR IntrSlot[32];    /* There are 32 interrupt locations    */
}C55XX_INTR_TABLE;

/*---- ADC ------------------------------------------------------------------*/
typedef struct c55xx_ADC
{
    unsigned short adcr;            /* 00 */
    unsigned short addr;            /* 01 */
    unsigned short adcdr;           /* 02 */
    unsigned short adccr;           /* 03 */
}C55XX_ADC;

typedef ioport volatile C55XX_ADC *PC55XX_ADC;

/*---- External Bus Selection -----------------------------------------------*/
typedef struct c55xx_extbus
{
    unsigned short exbussel;           /* 00 */
}C55XX_EXTBUS;

typedef ioport volatile C55XX_EXTBUS *PC55XX_EXTBUS;

typedef volatile C55XX_INTR_TABLE * PC55XX_INTR_TABLE;

/*---- Define the base address for all the C55XX peripherals, io space-----*/
#define C55XX_RHEA_ADDR         0x0000
#define C55XX_BOOT_ADDR         0x000F
#define C55XX_EMIF_ADDR         0x0800
#define C55XX_CACHE_ADDR        0x1400
#define C55XX_DMA_GLB_ADDR      0x0E00
#define C55XX_DMA0_ADDR         0x0C00
#define C55XX_DMA1_ADDR         0x0C20
#define C55XX_DMA2_ADDR         0x0C40
#define C55XX_DMA3_ADDR         0x0C60
#define C55XX_DMA4_ADDR         0x0C80
#define C55XX_DMA5_ADDR         0x0CA0
#define C55XX_TRACE_ADDR        0x2000
#define C55XX_CLKMD_ADDR        0x1C00
#define C55XX_USBCLKMD_ADDR     0x1E00
#define C55XX_TIM0_ADDR         0x1000
#define C55XX_TIM1_ADDR         0x2400
#define C55XX_RTC_ADDR          0x1800
#define C55XX_MSP0_ADDR         0x2800
#define C55XX_MSP1_ADDR         0x2C00
#define C55XX_MSP2_ADDR         0x3000
#define C55XX_GPIO_ADDR         0x3400
#define C55XX_DIEID_ADDR        0x3800
#define C55XX_REVID_ADDR        0x3804
#define C55XX_I2C_ADDR          0x3c00
#define C55XX_ADC_ADDR          0x6800
#define C55XX_EXTBUS_ADDR       0x6C00

#define C55XX_MS0_ADDR           0x5000
#define C55XX_MS1_ADDR           0x5400
#define C55XX_MMC0_ADDR          0x4800
#define C55XX_MMC1_ADDR          0x4c00

/* Define the base address in data space for each external memory block-----*/
#define CE0_BASE          ( 0x050000L >> 1 )
#define CE1_BASE          ( 0x400000L >> 1 )
#define CE2_BASE          ( 0x800000L >> 1 )
#define CE3_BASE          ( 0xC00000L >> 1 )

/*---- Non-Maskable interrupts: Sxx are software ints-----------------------*/
#define INTR_RS             0
#define INTR_NMI            1
#define INTR_INT0           2
#define INTR_INT2           3
#define INTR_TINT0          4
#define INTR_RINT0          5
#define INTR_RINT1          6
#define INTR_XINT1          7
#define INTR_SINT8          8
#define INTR_DMAC1          9
#define INTR_DSPINT         10
#define INTR_INT3           11
#define INTR_RINT2          12
#define INTR_XINT2          13
#define INTR_DMAC4          14
#define INTR_DMAC5          15
#define INTR_INT1           16
#define INTR_XINT0          17
#define INTR_DMAC0          18
#define INTR_INT4           19
#define INTR_DMAC2          20
#define INTR_DMAC3          21
#define INTR_TINT1          22
#define INTR_INT5           23
#define INTR_BERR           24
#define INTR_DLOG           25
#define INTR_RTOS           26
#define INTR_SINT27         27
#define INTR_SINT28         28
#define INTR_SINT29         29
#define INTR_SINT30         30
#define INTR_SINT31         31

/*---- global data declarations --------------------------------------------*/

/*---- global function prototypes ------------------------------------------*/

#endif /*  c55xxdefs_h ---- END OF FILE ------------------------------------*/

