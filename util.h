
#define DSP_CLKIN        12
#define NULLLOOP_CLK     12

typedef struct {
    int freq;        // DSP operating clock
    int clkin;       // DSP input clock
    int pllmult;     // PLL multiplier
    int plldiv;      // PLL divisor
    int clksperusec; // DSP clocks per usec
    int nullloopclk; // DSP clocks per null loop
} DSPCLK;

extern DSPCLK dspclk;

#define Read(addr) addr
#define Write(addr,data) addr = data
#define ReadMask(addr,mask) (addr & (mask))
#define WriteMask(addr,data,mask) addr = (addr & ~(mask)) | (data)
#define ClearMask(addr,mask) addr = (addr & ~(mask))
#define SetMask(addr,mask) addr = (addr | (mask))
#define ReadField(addr,mask) ((addr & (mask)) >> firstbit(mask))
#define WriteField(addr,data,mask) addr = (addr & ~(mask)) | (data << firstbit(mask))

extern int firstbit(unsigned short mask);

void INT_Disable(unsigned short mask);
void INT_DisableGlobal();
void INT_SetVec(unsigned long addr);
void PLL_Init(int freq);
