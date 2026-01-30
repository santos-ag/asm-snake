/* crt0.S - Startup Code para RV32I */
.section .text.init
.global _start

_start:
    /* --- 1. INICIALIZAR PILHA --- */
    la sp, _stack_top

    /* --- 2. INICIALIZAR .DATA (copiar de ROM para RAM) --- */
    la t0, _data_load     /* Endereço dos dados na ROM */
    la t1, _data_start    /* Endereço de destino na RAM */
    la t2, _data_end      /* Fim da seção .data */

copy_data:
    beq t1, t2, init_bss  /* Se já copiou tudo, vai pro BSS */
    lw t3, 0(t0)          /* Lê da ROM */
    sw t3, 0(t1)          /* Escreve na RAM */
    addi t0, t0, 4
    addi t1, t1, 4
    j copy_data

init_bss:
    /* --- 3. LIMPAR BSS (zerar variáveis não inicializadas) --- */
    la t0, _bss_start
    la t1, _bss_end

clear_bss:
    beq t0, t1, call_main /* Se terminou, vai pro main */
    sw zero, 0(t0)
    addi t0, t0, 4
    j clear_bss

call_main:
    /* --- 4. CHAMAR MAIN --- */
    jal ra, main

    /* --- 5. LOOP INFINITO (se main retornar) --- */
exit:
    j exit

.size _start, . - _start
