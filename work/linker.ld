ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x0000, LENGTH = 0x00010000
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text :
    {
        *(.text*)
    } > RAM

    /* primeiro endereço livre imediatamente após .text */
    _ram_base = .;

    . = ALIGN(4);
    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}

