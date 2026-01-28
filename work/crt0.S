.section .text
.global _start
_start:
    la sp, _stack_top
    la t0, _ram_base

    li t1, 1
    sw t1, 0(t0)

    li t1, 2
    sw t1, 4(t0)

    li t1, 3
    sw t1, 8(t0)

done:
    j done
