/* crt0.S - Startup Code para RV32I */
.section .text.init
.global _start
.global _bss_start
.global _bss_end

_start:
    /* 1. DESABILITAR INTERRUPÇÕES (se houver) */
    /* Não necessário para RV32I base sem extensões */
    
    /* 2. INICIALIZAR STACK POINTER */
    lui sp, 0x10              # sp = 0x10000 (64KB, topo da RAM)
    
    /* 3. INICIALIZAR GLOBAL POINTER (opcional) */
    /* lui gp, %hi(__global_pointer$) */
    /* addi gp, gp, %lo(__global_pointer$) */
    
    /* 4. CARREGAR ENDEREÇOS BSS */
    lui t0, %hi(_bss_start)
    addi t0, t0, %lo(_bss_start)
    lui t1, %hi(_bss_end)
    addi t1, t1, %lo(_bss_end)
    
    /* 5. LIMPAR BSS (zerar variáveis não inicializadas) */
clear_bss:
    beq t0, t1, call_main     # Se t0 == t1, terminou
    sw zero, 0(t0)            # Escreve 0
    addi t0, t0, 4            # Próxima word
    j clear_bss               # Repete
    
call_main:
    /* 6. CHAMAR MAIN */
    jal ra, main
    
    /* 7. LOOP INFINITO (se main retornar) */
exit:
    j exit

.size _start, . - _start
