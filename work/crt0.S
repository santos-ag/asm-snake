/* crt0.S - Startup Code para RV32I */
.section .text.init
.global _start

_start:
    /* 1. INICIALIZAR STACK POINTER */
    lui sp, %hi(_stack_top)
    addi sp, sp, %lo(_stack_top)
    
    /* 2. CARREGAR ENDEREÇOS BSS */
    lui t0, %hi(_bss_start)
    addi t0, t0, %lo(_bss_start)
    lui t1, %hi(_bss_end)
    addi t1, t1, %lo(_bss_end)
    
    /* 3. LIMPAR BSS */
clear_bss:
    beq t0, t1, call_main
    sw zero, 0(t0)
    addi t0, t0, 4
    j clear_bss
    
call_main:
    /* 4. CHAMAR MAIN */
    jal ra, main
    
    /* 5. LOOP INFINITO */
exit:
    j exit

.size _start, . - _start
