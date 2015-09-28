default rel
global _diff_asm
global diff_asm


section .data
mask_rot: DB 1, 2, 0, 0xFF, 5, 6, 4, 0xFF, 9, 10, 8, 0xFF, 13, 14, 12, 0xFF

section .text
;void diff_asm (unsigned char *src, unsigned char *src2,
;              (       RDI        ,        RSI         ,        
;               unsigned char *dst, int filas, int cols)
;                      RDX        ,    RCX   ,    R8   )
diff_asm:
  push rbp
  mov rbp, rsp
  xor r10, r10
  movdqu xmm2, [mask_rot]
  ; suponemos que van a ser pares la cantidad total de columnas. Si
  ; así no fuera, el formato viene comprimido y tampoco serviría la
  ; signatura propuesta
  .filas:
    xor r9, r9
    .columnas:

      movdqu xmm0, [rdi]  ; XMM0 <- i1_a0 | i1_r0 | i1_g0 | i1_b0 | i1_a1 | i1_r1 | i1_g1 | i1_b1 | i1_a2 | i1_r2 | i1_g2 | i1_b2 | i1_a3 | i1_r3 | i1_g3 | i1_b3
      movdqu xmm3, [rsi]  ; XMM3 <- i2_a0 | i2_r0 | i2_g0 | i2_b0 | i2_a1 | i2_r1 | i2_g1 | i2_b1 | i2_a2 | i2_r2 | i2_g2 | i2_b2 | i2_a3 | i2_r3 | i2_g3 | i2_b3

    					  ; XMM0 : imagen 1
    					  ; XMM3 : imagen 2

      movdqu xmm1, xmm0   ; XMM1 <- XMM0
      pmaxub xmm0, xmm3	  ; XMM0 <- MAX(i1_a0, i2_a0) | MAX(i1_r0, i2_r0) | MAX(i1_g0, i2_g0) | MAX(i1_b0, i2_b0) | MAX(i1_a1, i2_a1) | MAX(i1_r1, i2_r1) | MAX(i1_g1, i2_g1) | MAX(i1_b1, i2_b1) | MAX(i1_a2, i2_a2) | MAX(i1_r2, i2_r2) | MAX(i1_g2, i2_g2) | MAX(i1_b3, i2_b2) | MAX(i1_a3, i2_a3) | MAX(i1_r3, i2_r3) | MAX(i1_g3, i2_g3) | MAX(i1_b3 i2_b3)
      pminub xmm1, xmm3   ; XMM1 <- MIN(i1_a0, i2_a0) | MIN(i1_r0, i2_r0) | MIN(i1_g0, i2_g0) | MIN(i1_b0, i2_b0) | MIN(i1_a1, i2_a1) | MIN(i1_r1, i2_r1) | MIN(i1_g1, i2_g1) | MIN(i1_b1, i2_b1) | MIN(i1_a2, i2_a2) | MIN(i1_r2, i2_r2) | MIN(i1_g2, i2_g2) | MIN(i1_b3, i2_b2) | MIN(i1_a3, i2_a3) | MIN(i1_r3, i2_r3) | MIN(i1_g3, i2_g3) | MIN(i1_b3 i2_b3)
      ; ahora xmm1 tiene los bajos y xmm0 los altos

      psubb xmm0, xmm1	  ; XMM0 -= XMM1
      movdqu xmm1, xmm0	  ; XMM1 <- XMM0

      					  ; XXM1 = max(a0) - min(a0) | max(r0) - min(r0) | max(g0) - min(g0) | max(b0) - min(b0) | max(a1) - min(a1) | max(r1) - min(r1) | max(g1) - min(g1) | max(b1) - min(b1) | max(a2) - min(a2) | max(r2) - min(r2) | max(g2) - min(g2) | max(b2) - min(b2) | max(a3) - min(a3) | max(r3) - min(r3) | max(g3) - min(g3) | max(b3) - min(b3)

      ; el máximo en unas rotaciones
      pshufb xmm1, xmm2
      pmaxub xmm0, xmm1   ; 
      pshufb xmm1, xmm2
      pmaxub xmm0, xmm1 

      ; guardo y escribo el valor de alpha
      movdqu [rdx], xmm0
      mov byte [rdx+3], 0xFF
      mov byte [rdx+7], 0xFF
      mov byte [rdx+11], 0xFF
      mov byte [rdx+15], 0xFF
      add rsi, 16
      add rdi, 16
      add rdx, 16
      ; esto es un poco más rápido (10%) que sumar en un registro
      ; extra y acceder con [ ] a la posición de memoria con
      ; desplazamiento
      add r9, 4                 ; procesamos cuatro pixeles a la vez
      cmp r9, r8
      jnz .columnas
    inc r10
    cmp r10, rcx
    jnz .filas

  pop rbp
  ret
