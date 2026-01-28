    .section .text
    .global _start
    .type _start, @function

_start:
    la   sp, _stack_top

    la   t0, _ram_base

    li   t1, 0x11111111
    sw   t1, 0(t0)

    li   t1, 5
    li   t2, 7
    add  t3, t1, t2
    sw   t3, 4(t0)

    sub  t3, t2, t1
    sw   t3, 8(t0)

    slli t3, t1, 2
    sw   t3, 12(t0)

    srli t3, t3, 1
    sw   t3, 16(t0)

    and  t3, t1, t2
    sw   t3, 20(t0)

    or   t3, t1, t2
    sw   t3, 24(t0)

    xor  t3, t1, t2
    sw   t3, 28(t0)

    slt  t3, t1, t2
    sw   t3, 32(t0)

    sltu t3, t2, t1
    sw   t3, 36(t0)

    beq  t1, t1, branch_ok
    li   t3, 1
    sw   t3, 40(t0)
    j    hang

branch_ok:
    li   t3, 0x22222222
    sw   t3, 40(t0)

    jal  ra, func
    sw   a0, 44(t0)

    lw   t3, 0(t0)
    sw   t3, 48(t0)

    li   t3, 0xCAFEBABE
    sw   t3, 52(t0)

hang:
    j hang

func:
    li   a0, 0x33333333
    ret

