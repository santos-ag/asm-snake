/* linker.ld - Script de Linkagem para RV32I */

ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 0x10000  /* 64KB */
}

/* Símbolos importantes */
_stack_size = 0x1000;  /* 4KB de stack */

SECTIONS
{
    /* ========== CÓDIGO ========== */
    .text 0x00000000 : ALIGN(4)
    {
        _text_start = .;
        *(.text.init)        /* crt0.S PRIMEIRO! */
        *(.text .text.*)     /* Resto do código */
        _text_end = .;
    } > RAM
    
    /* ========== DADOS SOMENTE LEITURA ========== */
    .rodata : ALIGN(4)
    {
        _rodata_start = .;
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        _rodata_end = .;
    } > RAM
    
    /* ========== DADOS INICIALIZADOS ========== */
    .data : ALIGN(4)
    {
        _data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        _data_end = .;
    } > RAM
    
    /* ========== DADOS NÃO INICIALIZADOS ========== */
    .bss : ALIGN(4)
    {
        _bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        _bss_end = .;
    } > RAM
    
    /* ========== HEAP E STACK ========== */
    . = ALIGN(16);
    _heap_start = .;
    
    /* Stack no final da RAM */
    _stack_bottom = ORIGIN(RAM) + LENGTH(RAM) - _stack_size;
    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
    
    /* Verificações */
    ASSERT(_bss_end <= _stack_bottom, "Programa muito grande! Colide com stack.")
    ASSERT(_text_start == 0x00000000, "Código não começa em 0x00000000!")
    
    /* ========== DESCARTE ========== */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.riscv.attributes)
    }
}
