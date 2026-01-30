ENTRY(_start)

MEMORY
{
    /* 64KB de RAM (ajuste conforme seu Logisim) */
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 0x10000
}

SECTIONS
{
    /* 1. CÓDIGO (.text) - Começa em 0x0000 */
    .text : ALIGN(4)
    {
        *(.text.init)    /* crt0.S primeiro */
        *(.text*)
        *(.text.*)
    } > RAM

    /* 2. DADOS SOMENTE LEITURA (.rodata) */
    .rodata : ALIGN(4)
    {
        *(.rodata*)
        *(.rodata.*)
    } > RAM

    /* 3. DADOS INICIALIZADOS (.data) */
    /* Armazena na ROM, mas roda na RAM */
    .data : ALIGN(4)
    {
        _data_start = .;
        *(.data*)
        *(.data.*)
        *(.sdata*)
        _data_end = .;
    } > RAM

    _data_load = LOADADDR(.data);

    /* 4. DADOS NÃO INICIALIZADOS (.bss) */
    .bss : ALIGN(4)
    {
        _bss_start = .;
        *(.bss*)
        *(.bss.*)
        *(.sbss*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    /* 5. PILHA (Stack) */
    . = ALIGN(16);
    _heap_start = .;
    . = ORIGIN(RAM) + LENGTH(RAM) - 0x100; /* Reserva 256 bytes para stack */
    _stack_bottom = .;
    . = ORIGIN(RAM) + LENGTH(RAM);
    _stack_top = .;

    /* Descarta seções desnecessárias */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note*)
    }
}
