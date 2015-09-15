default rel
global _diff_asm
global diff_asm


section .data
mask_rot: DB 1, 2, 0, 0xFF, 5, 6, 4, 0xFF

section .text
;void diff_asm (unsigned char *src, unsigned char *src2,
;              (       RDI        ,        RSI         ,        
;               unsigned char *dst, int filas, int cols)
;                      RDX        ,    RCX   ,    R8   )
diff_asm:
  push rbp
  mov rbp, rsp
  xor r10, r10
  movq xmm2, [mask_rot]
  .filas:
    xor r9, r9
    .columnas:
      movq xmm0, [rdi]
      movq xmm1, [rsi]
      psubb xmm0, xmm1
      pabsb xmm0, xmm0
      movq xmm1, xmm0
      pshufb xmm1, xmm2
      pmaxub xmm0, xmm1 
      pshufb xmm1, xmm2
      pmaxub xmm0, xmm1 

      movq [rdx], xmm0

      add rsi, 8
      add rdi, 8
      add rdx, 8
      add r9, 2                 ; procesamos dos pixeles a la vez
      cmp r9, r8
      jnz .columnas
    inc r10
    cmp r10, rcx
    jnz .filas

  pop rbp
  ret
