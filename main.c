/**
 *  @file           main.c
 *  @brief          Filterung eines stereo Audio-Signals
 *  @details        A/D-Wandlung eines stereo Audio Signals mit den TLV320AIC23B
 *                  Audio Codec, �bertragung der Werte via McBSP an den DSP,
 *                  Faltung (Berechnung) im DSP, senden der berechneten Daten via
 *                  McBSP zur�ck an den Audio Codec, D/A-Wandlung und Ausgabe.
 *
 *  @author         Daniel Becker
 *  @date           2018-12-20
 *
 *  @version        V0.00.00-0000 (2018-11-08)
 *                  - Datei erstellt
 *  @version        V0.00.01-0000 (2018-12-03)
 *                  - Kommunikation und Konfiguration des AIC23B Audio Codec
 *                  - Verz�gerungs- bzw. Delayfunktion eingef�gt
 *                  - doxygen-f�hige Dokumentation des Quellcode
 *  @version        V0.00.02-0000 (2018-12-18)
 *                  - Konfiguration des McBSP angepasst
 *                  - Input = Output, Messungen im Labor f_g ~ 28 kHz
 *                  - TBD Faltung (Berechnung der Filter)
 *  @version        V0.01.00-0000 (2018-12-20)
 *                  - Lauff�hig, Mono, 13 Filterkoeffizienten
 *  @version        V0.01.01-0000 (2018-12-20)
 *                  - Erweiterung auf 61 Koeffizienten
 *  @version        V0.01.02-0000 (2018-12-20)
 *                  - Ein-/Ausschalten der LED0 Synchron zum Sprung in die ISR
 *                  - Compiler-Optimierung Level 3 (-o3) f�hrt zur Reduzierung
 *                    der ISR Laufzeit von 14us (68%) auf 4us (18%)
 */

// --- I N C L U D E S ---------------------------------------------------------
#include    <stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include    "stdint.h"

// .-. C H I P   S U P P O R T   L I B R A R Y -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#include    <csl.h>
#include    <csl_irq.h>
#include    <csl_mcbsp.h>
#include    <csl_timer.h>
#include    <csl_i2c.h>

// Own modules
#include "util.h"


// --- D E F I N I T I O N E N -------------------------------------------------
/** Gr��e des/der Ringspeicher */
#define     SIZE_OF_BUFFER      1024

// ... K O N F I G U R A T I O N E N ( cfg_ ) ..................................
/** Konfigurationsstuktur f�r den McBSP (multi-channel buffered serial port   */
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

//---------Global data definition---------
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

/** Konfigurationsstruktur f�r den I2C-Bus                                    */
I2C_Setup cfg_I2C = {
    0,                          /*< Address Mode */
    0x0000,                     /*< Own Address */
    144,                        /*< System Clock Value (MHz) */
    100,                        /*< Desired Transfer rate ( 10 .. 400 kbps) */
    0,                          /*< Number of bits per byte to be received or transmitted */
    0,                          /*< Data Loopback mode */
    1                           /*< emulator FREE mode */
};

// --- G L O B A L E   V A R I A B L E N ---------------------------------------
//extern void c_int00( void );
//extern void VECSTART( void );
extern unsigned short USBTest_Init();
int volatile * const LEDs = (int *) 0x3F0000;

/** Ringspeicher f�r die empfangenen Daten */
volatile int cnt_input = 0;

//https://www.daycounter.com/Calculators/Sine-Generator-Calculator.phtml
 const int sin_LT[SIZE_OF_BUFFER] = {
                     32768 - 32768,     32969 - 32768,  33170 - 32768,  33371 - 32768,  33572 - 32768,  33774 - 32768,  33975 - 32768,  34176 - 32768,
                     34377 - 32768,     34578 - 32768,  34779 - 32768,  34980 - 32768,  35180 - 32768,  35381 - 32768,  35582 - 32768,  35782 - 32768,
                     35982 - 32768,     36183 - 32768,  36383 - 32768,  36583 - 32768,  36782 - 32768,  36982 - 32768,  37182 - 32768,  37381 - 32768,
                     37580 - 32768,     37779 - 32768,  37978 - 32768,  38177 - 32768,  38375 - 32768,  38573 - 32768,  38771 - 32768,  38969 - 32768,
                     39166 - 32768,     39364 - 32768,  39561 - 32768,  39757 - 32768,  39954 - 32768,  40150 - 32768,  40346 - 32768,  40542 - 32768,
                     40737 - 32768,     40932 - 32768,  41127 - 32768,  41321 - 32768,  41515 - 32768,  41709 - 32768,  41903 - 32768,  42096 - 32768,
                     42288 - 32768,     42481 - 32768,  42673 - 32768,  42864 - 32768,  43056 - 32768,  43247 - 32768,  43437 - 32768,  43627 - 32768,
                     43817 - 32768,     44006 - 32768,  44195 - 32768,  44383 - 32768,  44571 - 32768,  44759 - 32768,  44946 - 32768,  45133 - 32768,
                     45319 - 32768,     45504 - 32768,  45690 - 32768,  45874 - 32768,  46058 - 32768,  46242 - 32768,  46425 - 32768,  46608 - 32768,
                     46790 - 32768,     46972 - 32768,  47153 - 32768,  47333 - 32768,  47513 - 32768,  47693 - 32768,  47872 - 32768,  48050 - 32768,
                     48228 - 32768,     48405 - 32768,  48582 - 32768,  48758 - 32768,  48933 - 32768,  49108 - 32768,  49282 - 32768,  49455 - 32768,
                     49628 - 32768,     49800 - 32768,  49972 - 32768,  50143 - 32768,  50313 - 32768,  50483 - 32768,  50652 - 32768,  50820 - 32768,
                     50988 - 32768,     51155 - 32768,  51321 - 32768,  51487 - 32768,  51651 - 32768,  51815 - 32768,  51979 - 32768,  52142 - 32768,
                     52303 - 32768,     52465 - 32768,  52625 - 32768,  52785 - 32768,  52944 - 32768,  53102 - 32768,  53259 - 32768,  53416 - 32768,
                     53572 - 32768,     53727 - 32768,  53881 - 32768,  54035 - 32768,  54188 - 32768,  54339 - 32768,  54491 - 32768,  54641 - 32768,
                     54790 - 32768,     54939 - 32768,  55087 - 32768,  55234 - 32768,  55380 - 32768,  55525 - 32768,  55669 - 32768,  55813 - 32768,
                     55955 - 32768,     56097 - 32768,  56238 - 32768,  56378 - 32768,  56517 - 32768,  56655 - 32768,  56793 - 32768,  56929 - 32768,
                     57065 - 32768,     57199 - 32768,  57333 - 32768,  57466 - 32768,  57597 - 32768,  57728 - 32768,  57858 - 32768,  57987 - 32768,
                     58115 - 32768,     58242 - 32768,  58368 - 32768,  58493 - 32768,  58618 - 32768,  58741 - 32768,  58863 - 32768,  58984 - 32768,
                     59104 - 32768,     59224 - 32768,  59342 - 32768,  59459 - 32768,  59575 - 32768,  59691 - 32768,  59805 - 32768,  59918 - 32768,
                     60030 - 32768,     60141 - 32768,  60251 - 32768,  60360 - 32768,  60468 - 32768,  60575 - 32768,  60681 - 32768,  60786 - 32768,
                     60890 - 32768,     60993 - 32768,  61095 - 32768,  61195 - 32768,  61295 - 32768,  61393 - 32768,  61491 - 32768,  61587 - 32768,
                     61682 - 32768,     61776 - 32768,  61869 - 32768,  61961 - 32768,  62052 - 32768,  62142 - 32768,  62230 - 32768,  62318 - 32768,
                     62404 - 32768,     62490 - 32768,  62574 - 32768,  62657 - 32768,  62739 - 32768,  62820 - 32768,  62899 - 32768,  62978 - 32768,
                     63055 - 32768,     63131 - 32768,  63206 - 32768,  63280 - 32768,  63353 - 32768,  63425 - 32768,  63495 - 32768,  63565 - 32768,
                     63633 - 32768,     63700 - 32768,  63766 - 32768,  63830 - 32768,  63894 - 32768,  63956 - 32768,  64017 - 32768,  64077 - 32768,
                     64136 - 32768,     64193 - 32768,  64250 - 32768,  64305 - 32768,  64359 - 32768,  64412 - 32768,  64464 - 32768,  64514 - 32768,
                     64563 - 32768,     64611 - 32768,  64658 - 32768,  64704 - 32768,  64748 - 32768,  64791 - 32768,  64834 - 32768,  64874 - 32768,
                     64914 - 32768,     64952 - 32768,  64990 - 32768,  65025 - 32768,  65060 - 32768,  65094 - 32768,  65126 - 32768,  65157 - 32768,
                     65187 - 32768,     65216 - 32768,  65243 - 32768,  65269 - 32768,  65294 - 32768,  65318 - 32768,  65340 - 32768,  65362 - 32768,
                     65382 - 32768,     65401 - 32768,  65418 - 32768,  65435 - 32768,  65450 - 32768,  65464 - 32768,  65476 - 32768,  65488 - 32768,
                     65498 - 32768,     65507 - 32768,  65515 - 32768,  65521 - 32768,  65526 - 32768,  65530 - 32768,  65533 - 32768,  65535 - 32768,
                     65535 - 32768,     65534 - 32768,  65532 - 32768,  65528 - 32768,  65524 - 32768,  65518 - 32768,  65511 - 32768,  65503 - 32768,
                     65493 - 32768,     65482 - 32768,  65470 - 32768,  65457 - 32768,  65442 - 32768,  65427 - 32768,  65410 - 32768,  65391 - 32768,
                     65372 - 32768,     65351 - 32768,  65329 - 32768,  65306 - 32768,  65282 - 32768,  65256 - 32768,  65230 - 32768,  65201 - 32768,
                     65172 - 32768,     65142 - 32768,  65110 - 32768,  65077 - 32768,  65043 - 32768,  65008 - 32768,  64971 - 32768,  64933 - 32768,
                     64894 - 32768,     64854 - 32768,  64813 - 32768,  64770 - 32768,  64726 - 32768,  64681 - 32768,  64635 - 32768,  64587 - 32768,
                     64539 - 32768,     64489 - 32768,  64438 - 32768,  64386 - 32768,  64332 - 32768,  64278 - 32768,  64222 - 32768,  64165 - 32768,
                     64107 - 32768,     64047 - 32768,  63987 - 32768,  63925 - 32768,  63862 - 32768,  63798 - 32768,  63733 - 32768,  63666 - 32768,
                     63599 - 32768,     63530 - 32768,  63460 - 32768,  63389 - 32768,  63317 - 32768,  63244 - 32768,  63169 - 32768,  63093 - 32768,
                     63017 - 32768,     62939 - 32768,  62860 - 32768,  62779 - 32768,  62698 - 32768,  62616 - 32768,  62532 - 32768,  62447 - 32768,
                     62361 - 32768,     62274 - 32768,  62186 - 32768,  62097 - 32768,  62007 - 32768,  61915 - 32768,  61823 - 32768,  61729 - 32768,
                     61635 - 32768,     61539 - 32768,  61442 - 32768,  61344 - 32768,  61245 - 32768,  61145 - 32768,  61044 - 32768,  60942 - 32768,
                     60838 - 32768,     60734 - 32768,  60629 - 32768,  60522 - 32768,  60415 - 32768,  60306 - 32768,  60196 - 32768,  60086 - 32768,
                     59974 - 32768,     59862 - 32768,  59748 - 32768,  59633 - 32768,  59517 - 32768,  59401 - 32768,  59283 - 32768,  59164 - 32768,
                     59044 - 32768,     58924 - 32768,  58802 - 32768,  58679 - 32768,  58556 - 32768,  58431 - 32768,  58305 - 32768,  58179 - 32768,
                     58051 - 32768,     57923 - 32768,  57793 - 32768,  57663 - 32768,  57532 - 32768,  57399 - 32768,  57266 - 32768,  57132 - 32768,
                     56997 - 32768,     56861 - 32768,  56724 - 32768,  56586 - 32768,  56448 - 32768,  56308 - 32768,  56168 - 32768,  56026 - 32768,
                     55884 - 32768,     55741 - 32768,  55597 - 32768,  55452 - 32768,  55307 - 32768,  55160 - 32768,  55013 - 32768,  54865 - 32768,
                     54716 - 32768,     54566 - 32768,  54415 - 32768,  54264 - 32768,  54111 - 32768,  53958 - 32768,  53804 - 32768,  53650 - 32768,
                     53494 - 32768,     53338 - 32768,  53181 - 32768,  53023 - 32768,  52864 - 32768,  52705 - 32768,  52545 - 32768,  52384 - 32768,
                     52223 - 32768,     52060 - 32768,  51897 - 32768,  51734 - 32768,  51569 - 32768,  51404 - 32768,  51238 - 32768,  51071 - 32768,
                     50904 - 32768,     50736 - 32768,  50568 - 32768,  50398 - 32768,  50228 - 32768,  50058 - 32768,  49886 - 32768,  49714 - 32768,
                     49542 - 32768,     49369 - 32768,  49195 - 32768,  49020 - 32768,  48845 - 32768,  48670 - 32768,  48493 - 32768,  48317 - 32768,
                     48139 - 32768,     47961 - 32768,  47782 - 32768,  47603 - 32768,  47424 - 32768,  47243 - 32768,  47062 - 32768,  46881 - 32768,
                     46699 - 32768,     46517 - 32768,  46334 - 32768,  46150 - 32768,  45966 - 32768,  45782 - 32768,  45597 - 32768,  45412 - 32768,
                     45226 - 32768,     45039 - 32768,  44852 - 32768,  44665 - 32768,  44477 - 32768,  44289 - 32768,  44101 - 32768,  43912 - 32768,
                     43722 - 32768,     43532 - 32768,  43342 - 32768,  43151 - 32768,  42960 - 32768,  42769 - 32768,  42577 - 32768,  42385 - 32768,
                     42192 - 32768,     41999 - 32768,  41806 - 32768,  41612 - 32768,  41418 - 32768,  41224 - 32768,  41029 - 32768,  40835 - 32768,
                     40639 - 32768,     40444 - 32768,  40248 - 32768,  40052 - 32768,  39856 - 32768,  39659 - 32768,  39462 - 32768,  39265 - 32768,
                     39068 - 32768,     38870 - 32768,  38672 - 32768,  38474 - 32768,  38276 - 32768,  38077 - 32768,  37879 - 32768,  37680 - 32768,
                     37481 - 32768,     37281 - 32768,  37082 - 32768,  36882 - 32768,  36683 - 32768,  36483 - 32768,  36283 - 32768,  36083 - 32768,
                     35882 - 32768,     35682 - 32768,  35481 - 32768,  35281 - 32768,  35080 - 32768,  34879 - 32768,  34678 - 32768,  34477 - 32768,
                     34276 - 32768,     34075 - 32768,  33874 - 32768,  33673 - 32768,  33472 - 32768,  33271 - 32768,  33069 - 32768,  32868 - 32768,
                     32667 - 32768,     32466 - 32768,  32264 - 32768,  32063 - 32768,  31862 - 32768,  31661 - 32768,  31460 - 32768,  31259 - 32768,
                     31058 - 32768,     30857 - 32768,  30656 - 32768,  30455 - 32768,  30254 - 32768,  30054 - 32768,  29853 - 32768,  29653 - 32768,
                     29452 - 32768,     29252 - 32768,  29052 - 32768,  28852 - 32768,  28653 - 32768,  28453 - 32768,  28254 - 32768,  28054 - 32768,
                     27855 - 32768,     27656 - 32768,  27458 - 32768,  27259 - 32768,  27061 - 32768,  26863 - 32768,  26665 - 32768,  26467 - 32768,
                     26270 - 32768,     26073 - 32768,  25876 - 32768,  25679 - 32768,  25483 - 32768,  25287 - 32768,  25091 - 32768,  24896 - 32768,
                     24700 - 32768,     24506 - 32768,  24311 - 32768,  24117 - 32768,  23923 - 32768,  23729 - 32768,  23536 - 32768,  23343 - 32768,
                     23150 - 32768,     22958 - 32768,  22766 - 32768,  22575 - 32768,  22384 - 32768,  22193 - 32768,  22003 - 32768,  21813 - 32768,
                     21623 - 32768,     21434 - 32768,  21246 - 32768,  21058 - 32768,  20870 - 32768,  20683 - 32768,  20496 - 32768,  20309 - 32768,
                     20123 - 32768,     19938 - 32768,  19753 - 32768,  19569 - 32768,  19385 - 32768,  19201 - 32768,  19018 - 32768,  18836 - 32768,
                     18654 - 32768,     18473 - 32768,  18292 - 32768,  18111 - 32768,  17932 - 32768,  17753 - 32768,  17574 - 32768,  17396 - 32768,
                     17218 - 32768,     17042 - 32768,  16865 - 32768,  16690 - 32768,  16515 - 32768,  16340 - 32768,  16166 - 32768,  15993 - 32768,
                     15821 - 32768,     15649 - 32768,  15477 - 32768,  15307 - 32768,  15137 - 32768,  14967 - 32768,  14799 - 32768,  14631 - 32768,
                     14464 - 32768,     14297 - 32768,  14131 - 32768,  13966 - 32768,  13801 - 32768,  13638 - 32768,  13475 - 32768,  13312 - 32768,
                     13151 - 32768,     12990 - 32768,  12830 - 32768,  12671 - 32768,  12512 - 32768,  12354 - 32768,  12197 - 32768,  12041 - 32768,
                     11885 - 32768,     11731 - 32768,  11577 - 32768,  11424 - 32768,  11271 - 32768,  11120 - 32768,  10969 - 32768,  10819 - 32768,
                     10670 - 32768,     10522 - 32768,  10375 - 32768,  10228 - 32768,  10083 - 32768,  9938 - 32768,   9794 - 32768,   9651 - 32768,
                     9509 - 32768,      9367 - 32768,   9227 - 32768,   9087 - 32768,   8949 - 32768,   8811 - 32768,   8674 - 32768,   8538 - 32768,
                     8403 - 32768,      8269 - 32768,   8136 - 32768,   8003 - 32768,   7872 - 32768,   7742 - 32768,   7612 - 32768,   7484 - 32768,
                     7356 - 32768,      7230 - 32768,   7104 - 32768,   6979 - 32768,   6856 - 32768,   6733 - 32768,   6611 - 32768,   6491 - 32768,
                     6371 - 32768,      6252 - 32768,   6134 - 32768,   6018 - 32768,   5902 - 32768,   5787 - 32768,   5673 - 32768,   5561 - 32768,
                     5449 - 32768,      5339 - 32768,   5229 - 32768,   5120 - 32768,   5013 - 32768,   4906 - 32768,   4801 - 32768,   4697 - 32768,
                     4593 - 32768,      4491 - 32768,   4390 - 32768,   4290 - 32768,   4191 - 32768,   4093 - 32768,   3996 - 32768,   3900 - 32768,
                     3806 - 32768,      3712 - 32768,   3620 - 32768,   3528 - 32768,   3438 - 32768,   3349 - 32768,   3261 - 32768,   3174 - 32768,
                     3088 - 32768,      3003 - 32768,   2919 - 32768,   2837 - 32768,   2756 - 32768,   2675 - 32768,   2596 - 32768,   2518 - 32768,
                     2442 - 32768,      2366 - 32768,   2291 - 32768,   2218 - 32768,   2146 - 32768,   2075 - 32768,   2005 - 32768,   1936 - 32768,
                     1869 - 32768,      1802 - 32768,   1737 - 32768,   1673 - 32768,   1610 - 32768,   1548 - 32768,   1488 - 32768,   1428 - 32768,
                     1370 - 32768,      1313 - 32768,   1257 - 32768,   1203 - 32768,   1149 - 32768,   1097 - 32768,   1046 - 32768,   996 - 32768,
                     948 - 32768,       900 - 32768,    854 - 32768,    809 - 32768,    765 - 32768,    722 - 32768,    681 - 32768,    641 - 32768,
                     602 - 32768,       564 - 32768,    527 - 32768,    492 - 32768,    458 - 32768,    425 - 32768,    393 - 32768,    363 - 32768,
                     334 - 32768,       305 - 32768,    279 - 32768,    253 - 32768,    229 - 32768,    206 - 32768,    184 - 32768,    163 - 32768,
                     144 - 32768,       125 - 32768,    108 - 32768,    93 - 32768,     78 - 32768,     65 - 32768,     53 - 32768,     42 - 32768,
                     32 - 32768,        24 - 32768,     17 - 32768,     11 - 32768,     7 - 32768,      3 - 32768,      1 - 32768,      0 - 32768,
                     0 - 32768,         2 - 32768,      5 - 32768,      9 - 32768,      14 - 32768,     20 - 32768,     28 - 32768,     37 - 32768,
                     47 - 32768,        59 - 32768,     71 - 32768,     85 - 32768,     100 - 32768,    117 - 32768,    134 - 32768,    153 - 32768,
                     173 - 32768,       195 - 32768,    217 - 32768,    241 - 32768,    266 - 32768,    292 - 32768,    319 - 32768,    348 - 32768,
                     378 - 32768,       409 - 32768,    441 - 32768,    475 - 32768,    510 - 32768,    545 - 32768,    583 - 32768,    621 - 32768,
                     661 - 32768,       701 - 32768,    744 - 32768,    787 - 32768,    831 - 32768,    877 - 32768,    924 - 32768,    972 - 32768,
                     1021 - 32768,      1071 - 32768,   1123 - 32768,   1176 - 32768,   1230 - 32768,   1285 - 32768,   1342 - 32768,   1399 - 32768,
                     1458 - 32768,      1518 - 32768,   1579 - 32768,   1641 - 32768,   1705 - 32768,   1769 - 32768,   1835 - 32768,   1902 - 32768,
                     1970 - 32768,      2040 - 32768,   2110 - 32768,   2182 - 32768,   2255 - 32768,   2329 - 32768,   2404 - 32768,   2480 - 32768,
                     2557 - 32768,      2636 - 32768,   2715 - 32768,   2796 - 32768,   2878 - 32768,   2961 - 32768,   3045 - 32768,   3131 - 32768,
                     3217 - 32768,      3305 - 32768,   3393 - 32768,   3483 - 32768,   3574 - 32768,   3666 - 32768,   3759 - 32768,   3853 - 32768,
                     3948 - 32768,      4044 - 32768,   4142 - 32768,   4240 - 32768,   4340 - 32768,   4440 - 32768,   4542 - 32768,   4645 - 32768,
                     4749 - 32768,      4854 - 32768,   4960 - 32768,   5067 - 32768,   5175 - 32768,   5284 - 32768,   5394 - 32768,   5505 - 32768,
                     5617 - 32768,      5730 - 32768,   5844 - 32768,   5960 - 32768,   6076 - 32768,   6193 - 32768,   6311 - 32768,   6431 - 32768,
                     6551 - 32768,      6672 - 32768,   6794 - 32768,   6917 - 32768,   7042 - 32768,   7167 - 32768,   7293 - 32768,   7420 - 32768,
                     7548 - 32768,      7677 - 32768,   7807 - 32768,   7938 - 32768,   8069 - 32768,   8202 - 32768,   8336 - 32768,   8470 - 32768,
                     8606 - 32768,      8742 - 32768,   8880 - 32768,   9018 - 32768,   9157 - 32768,   9297 - 32768,   9438 - 32768,   9580 - 32768,
                     9722 - 32768,      9866 - 32768,   10010 - 32768,  10155 - 32768,  10301 - 32768,  10448 - 32768,  10596 - 32768,  10745 - 32768,
                     10894 - 32768,     11044 - 32768,  11196 - 32768,  11347 - 32768,  11500 - 32768,  11654 - 32768,  11808 - 32768,  11963 - 32768,
                     12119 - 32768,     12276 - 32768,  12433 - 32768,  12591 - 32768,  12750 - 32768,  12910 - 32768,  13070 - 32768,  13232 - 32768,
                     13393 - 32768,     13556 - 32768,  13720 - 32768,  13884 - 32768,  14048 - 32768,  14214 - 32768,  14380 - 32768,  14547 - 32768,
                     14715 - 32768,     14883 - 32768,  15052 - 32768,  15222 - 32768,  15392 - 32768,  15563 - 32768,  15735 - 32768,  15907 - 32768,
                     16080 - 32768,     16253 - 32768,  16427 - 32768,  16602 - 32768,  16777 - 32768,  16953 - 32768,  17130 - 32768,  17307 - 32768,
                     17485 - 32768,     17663 - 32768,  17842 - 32768,  18022 - 32768,  18202 - 32768,  18382 - 32768,  18563 - 32768,  18745 - 32768,
                     18927 - 32768,     19110 - 32768,  19293 - 32768,  19477 - 32768,  19661 - 32768,  19845 - 32768,  20031 - 32768,  20216 - 32768,
                     20402 - 32768,     20589 - 32768,  20776 - 32768,  20964 - 32768,  21152 - 32768,  21340 - 32768,  21529 - 32768,  21718 - 32768,
                     21908 - 32768,     22098 - 32768,  22288 - 32768,  22479 - 32768,  22671 - 32768,  22862 - 32768,  23054 - 32768,  23247 - 32768,
                     23439 - 32768,     23632 - 32768,  23826 - 32768,  24020 - 32768,  24214 - 32768,  24408 - 32768,  24603 - 32768,  24798 - 32768,
                     24993 - 32768,     25189 - 32768,  25385 - 32768,  25581 - 32768,  25778 - 32768,  25974 - 32768,  26171 - 32768,  26369 - 32768,
                     26566 - 32768,     26764 - 32768,  26962 - 32768,  27160 - 32768,  27358 - 32768,  27557 - 32768,  27756 - 32768,  27955 - 32768,
                     28154 - 32768,     28353 - 32768,  28553 - 32768,  28753 - 32768,  28952 - 32768,  29152 - 32768,  29352 - 32768,  29553 - 32768,
                     29753 - 32768,     29953 - 32768,  30154 - 32768,  30355 - 32768,  30555 - 32768,  30756 - 32768,  30957 - 32768,  31158 - 32768,
                     31359 - 32768,     31560 - 32768,  31761 - 32768,  31963 - 32768,  32164 - 32768,  32365 - 32768,  32566 - 32768,  32768 - 32768
};


// ... H A N D L E R ( h_ ) ....................................................
/** Handler f�r McBSP0                                                        */
MCBSP_Handle h_McBSP;
/** Handler f�r Timer0                                                        */
TIMER_Handle mhTimer0;
TIMER_Handle mhTimer1;

// ... E V E N T   I D ( eID_ ) ................................................
/** Event ID des McBSP0-Rx-Interrupts                                         */
Uint16 eID_McBSP_rx;


Uint16 eventId0;
Uint16 eventId1;

uint16_t timPrecounter = 0;
volatile uint32_t ms = 0;

volatile Uint16 timer0_cnt = 0;
int old_intm;
uint16_t moduloValue = 13;
Uint16 tim_val;
uint32_t pos = 0;
uint16_t* CPLD = (uint16_t*) 0x3F0000;
uint16_t* PRD0 = (uint16_t*) 0x1001;
volatile uint32_t output = 0;

// --- F U N K T I O N S   P R O T O T Y P E N ---------------------------------
interrupt void timer0Isr(void);
interrupt void timer1Isr(void);

void fn_AIC23_config( void );
void fn_AIC23_reset( void );
int fn_AIC23_send( Uint16* data );
void fn_delay( Uint16 rounds );
uint16_t fToPRD(float f);
uint16_t tToPRD(float t);
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
    // E I N S T I E G S P U N K T   ( c_int00 ) .-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
   // void (*x)(void) = c_int00;
    // .-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

    EnableAPLL( );

    CSL_init();

    /* Set IVPH/IVPD to start of interrupt vector table */
    IRQ_setVecs(0x8000);

    /* Temporarily disable all maskable interrupts */
    old_intm = IRQ_globalDisable();

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
    IRQ_plug(eventId1,&timer1Isr);

    /* Konfiguration des I2C Bus                                              */
    I2C_setup( &cfg_I2C );

    /* Konfiguration des Audio Codec via I2C                                  */
    fn_AIC23_reset();
    fn_AIC23_config();

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
    PREG16(((TIMER_PrivateObj*)mhTimer0)->PrdAddr) = fToPRD(110.0 * SIZE_OF_BUFFER);
    TIMER_start(mhTimer0);
    TIMER_start(mhTimer1);





    /* Konfiguration des Timers (timer0)                                      */
    //h_Timer0 = TIMER_open( TIMER_DEV0, TIMER_OPEN_RESET );
    //eID_Timer0 = TIMER_getEventId( h_Timer0 );
    //TIMER_config( h_Timer0, &cfg_Timer0 );
    //TIMER_start( h_Timer0 );

    /* Aktivieren der Interrupts                                              */
    //IRQ_setVecs( (Uint32)&VECSTART );

    //IRQ_clear( eID_McBSP_rx );
    //IRQ_clear( eID_Timer0 );

    //IRQ_plug( eID_McBSP_rx, &isr_mcbsp_rx );
    //IRQ_plug( eID_Timer0, &isr_timer0 );

    //IRQ_enable( eID_McBSP_rx );
    //IRQ_enable( eID_Timer0 );

    //IRQ_globalEnable();
    output = 0x7FFF;
    USBTest_Init();
    //PLL_Init(48);

    /* Beginn der"Endlosschleife"                                            */
    while( 1 )
    {
        /*
        if(ms == 2000)
        {
            PREG16(((TIMER_PrivateObj*)mhTimer0)->PrdAddr) = fToPRD(220.0 * (float)SIZE_OF_BUFFER);
        }
        else if(ms == 4000 )
        {
            PREG16(((TIMER_PrivateObj*)mhTimer0)->PrdAddr) = fToPRD(440.0 * (float)SIZE_OF_BUFFER);
        }
        else if(ms == 6000 )
        {
            PREG16(((TIMER_PrivateObj*)mhTimer0)->PrdAddr) = fToPRD(880.0 * (float)SIZE_OF_BUFFER);
            ms = 0;
        }*/

        //PREG16(((TIMER_PrivateObj*)mhTimer0)->PrdAddr) += 1;
        //fn_delay(10);
    }

    // S T A T E M E N T   I S   U N R E A C H A B L E .-.-.-.-.-.-.-.-.-.-.-.-.
    TIMER_close( mhTimer0 );
    MCBSP_close( h_McBSP );
    return 0;
}

// --- F U N K T I O N E N -----------------------------------------------------
/**
 * @fn              fn_AIC23_config( void )
 * @brief           Konfiguration des AIC23B Audio Codecs via I2C
 * @details         Funktion um das "Control Interface" des TLV320AIC23B Audio
 *                  Codes via I2C-Bus anzusprechen. Die gew�nschten
 *                  Einstellungen werden �bertragen und in die entsprechenden
 *                  Register geschrieben.
 *  @note           MODE = @b 0 - Das "Control Interface" ist �ber TWI bzw. I2C
 *                  zu erreichen\n
 *                  CS = @b 0 - Die Adresse ist auf 0b0011010 festgelegt\n
 */
void fn_AIC23_config( void ) {
    /*
     * Aufbau des I2C Datenrahmen
     *
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | A | 15| 14| 13| 12| 11| 10| 9 | 8 | A | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | A |
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     * |I2C-Address................|R/W|ACK|Register-Address...........|   |ACK|                               |ACK|
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+   +
     * |                           |   |   |                           |Register-Data..........................|   |
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     */
    Uint16 *ptr;
    int i;

    /* Register Adressen und Daten f�r die I2C �bertragung                    */
    Uint16 cfg_AIC23[] = {
        0x001Fu,                /*< Left line input channel volume control (Address: 0000000) */
        0x021Fu,                /*< Right Line Input Channel Volume Control (Address: 0000001) */
        0x047Fu,                /*< Left Channel Headphone Volume Control (Address: 0000010) */
        0x067Fu,                /*< Right Channel Headphone Volume Control (Address: 0000011) */
        0x0812u,                /*< Analog Audio Path Control (Address: 0000100) */
        0x0A01u,                /*< Digital Audio Path Control (Address: 0000101) */
        0x0C02u,                /*< Power Down Control (Address: 0000110) */
        0x0E43u,                /*< Digital Audio Interface Format (Address: 0000111) */
        0x1001u,                /*< Sample Rate Control (Address: 0001000) */
        0x1201u                 /*< Digital Interface Activation (Address: 0001001) */
    };

    /* �bertragung der Konfiguration an das "Control Interface" per I2C-Bus   */
    for( i = 0; i < sizeof( cfg_AIC23 ); i++ )
    {
        ptr = cfg_AIC23 + i;
        fn_AIC23_send( ptr );
    }
}


/**
 *  @fn             fn_AIC23_reset( void )
 *  @brief          Reset Register des AIC23 "Control Interface" auf 0 0000 0000 setzen
 *  @details        Via I2C Bus wird das "Reset Register" (Adresse: 0b0001111)
 *                  des TLV320AIC23B Audio Codecs gel�scht, um so einen Reset
 *                  auszul�sen.
 *
 *  @return         @b 0 - bei erfolgreicher �bertragung des Reset Befehls\n
 *                  @b 1 - falls nach N Versuchen kein �bertragungsversuch erfolgreich war
 */
void fn_AIC23_reset( void )
{
    int n = 0;

    /* Anzahl der Wiederholungen, bevor die �bertragung abgebrochen wird      */
    int N = 5;

    /* Register Adresse + Daten um das "Reset Register" zu beschreiben        */
    Uint16 reset[] = { 0x1E00u };

    /* Ausf�hren von N �bertragungsversuchen                                  */
    while( n < N )
    {
        if( 0 == fn_AIC23_send( reset ) )
        {
            n = N;
        }

        n++;
    }

    /* Zeit f�r den AIC23 um den Reset durchzuf�hren                          */
    fn_delay( 50000 );
}

/**
 *  @fn             fn_AIC23_send( Uint16* data )
 *  @brief          Kommunikation mit dem "Control Interface" des AIC23
 *  @details        Funktion zur Kommunikation mit dem "Control Interface" des
 *                  TLV320AIC23B Audio Codecs. Der �bergebene, 16-Bit breite
 *                  Wert wird via I2C-Bus �bertragen.
 *  @note           7 Bit Register Adresse [15:9] + 9 Bit Register Daten [8:0]
 *
 *  @param[in]      data    16-Bit f�r die �bertragung via I2C (Adresse + Daten)
 *
 *  @return         @b 0 - erfolgreiche �bertragung\n
 *                  @b 1 - Bus belegt, Startsequenz kann nicht generiert werden\n
 *                  @b 2 - Timeout (beim ersten Byte)\n
 *                  @b 3 - NACK (not Acknowledge) empfangen\n
 *                  @b 4 - Timeout (beim Warten auf XRDY)\n
 *                  @b 5 - NACK (not Acknowledge) empfangen (beim letztem Byte)\n
 */
int fn_AIC23_send( Uint16* data )
{
    int S = I2C_write( data, 1, 1, 0x1Au, 1, 30000 );

//    S == 0 ? printf( "AIC23 send (0x%04X) via I2C done!\n", *data ) : printf( "AIC23 send (0x%04X) via I2C failed!\n", *data );

    fn_delay( 1 );

    return S;
}

/**
 *  @fn             fn_delay( Uint16 rounds )
 *  @brief          Warte-/Verz�gerungsfunktion
 *  @details        Funktion die eine Zeit lang "nichts tut" um so z.B. externen
 *                  Komponenten Zeit f�r einen Reset einzur�umen.
 *  @todo           fn_delay sollte in einem sp�teren Entwicklungsschritt durch
 *                  eine elegantere L�sung, z.B. mit einem Timer ersetzt werden,
 *                  um eine Nebenl�ufigkeit zu erm�glichen!
 *
 *  @param[in]      rounds  Anzahl der Wiederholungen
 */
void fn_delay( Uint16 rounds )
{
    Uint16 i, j;

    for( i = 0; i <= rounds; i++ )
    {
        for( j = 0; j <= 10; j++ )
        {
            asm( "\t nop" );
        }
    }
}

//f in Hz
uint16_t fToPRD(float f)
{
   return tToPRD(1/f);
}

//t in s
uint16_t tToPRD(float t)
{
    float clk = 192065280;
    uint32_t ticks = (float)(clk * t);
    return ticks / 16;
}

void setTone(float tone)
{
    PREG16(((TIMER_PrivateObj*)mhTimer0)->PrdAddr) = fToPRD(tone * (float)SIZE_OF_BUFFER);
}

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
    /*output = (unsigned) sin_LT[cnt_input]; //left channel
    output <<= 16;
    output |= (unsigned) sin_LT[cnt_input]; //right channel


    cnt_input = (cnt_input + 1) % SIZE_OF_BUFFER;
    */
    if ( ++cnt_input == SIZE_OF_BUFFER)
    {
        cnt_input = 0;
    }
    output = (unsigned) sin_LT[cnt_input];
}

interrupt void timer1Isr(void)
{
    timPrecounter++;
    if(timPrecounter >= 48)
    {
        ms++;
        timPrecounter = 0;
    }

    MCBSP_write32( h_McBSP, output );
}
