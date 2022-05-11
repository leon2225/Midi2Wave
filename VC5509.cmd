/* MEMORY directive      */
MEMORY
{
    RAM:     origin = 20000h    length = 20000h
    ROM:     origin = 00800h    length = 1f000h
}

/* SECTIONS directive    */
SECTIONS
{
    .text:    >   ROM
    .data:    >   RAM
    .cinit    >   ROM
    .bss:     >   RAM
    .stack    >   RAM
    .sysstack >   RAM
}
