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
        . = ALIGN(4);
        _text_end = .;
    } > RAM
    
    /* ========== DADOS SOMENTE LEITURA ========== */
    .rodata : ALIGN(4)
    {
        _rodata_start = .;
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        _rodata_end = .;
    } > RAM
    
    /* ========== DADOS INICIALIZADOS ========== */
    .data : ALIGN(4)
    {
        _data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _data_end = .;
    } > RAM
    
    /* ========== DADOS NÃO INICIALIZADOS ========== */
    .bss : ALIGN(4)
    {
        _bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > RAM
    
    /* ========== HEAP ========== */
    . = ALIGN(16);
    _heap_start = .;
    _heap_end = ORIGIN(RAM) + LENGTH(RAM) - _stack_size;  /* 0xF000 */
    
    /* ========== STACK ========== */
    /* Stack cresce de cima para baixo */
    _stack_bottom = ORIGIN(RAM) + LENGTH(RAM) - _stack_size;  /* 0xF000 */
    _stack_top = ORIGIN(RAM) + LENGTH(RAM);  /* 0x10000 */
    
    /* ========== VERIFICAÇÕES ========== */
    ASSERT(_bss_end <= _heap_end, "Erro: Programa muito grande! BSS colide com área de stack.")
    ASSERT(_heap_start <= _heap_end, "Erro: Sem espaço para heap!")
    ASSERT(_text_start == 0x00000000, "Erro: Código não começa em 0x00000000!")
    ASSERT(_stack_size >= 0x400, "Aviso: Stack muito pequena (mínimo 1KB recomendado).")
    
    /* ========== DESCARTE ========== */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.riscv.attributes)
    }
}
