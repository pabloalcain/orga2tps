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
  ; suponemos que van a ser pares la cantidad total de columnas. Si
  ; así no fuera, el formato viene comprimido y tampoco serviría la
  ; signatura propuesta
  .filas:
    xor r9, r9
    .columnas:
      movq xmm0, [rdi]
      movq xmm3, [rsi]

      movq xmm1, xmm0
      pmaxub xmm1, xmm3
      pminub xmm0, xmm3
      ; ahora xmm1 tiene los altos y xmm0 los bajos

      psubb xmm0, xmm1
      movq xmm1, xmm0

      ; el máximo en unas rotaciones
      pshufb xmm1, xmm2
      pmaxub xmm0, xmm1 
      pshufb xmm1, xmm2
      pmaxub xmm0, xmm1 

      ; guardo y escribo el valor de alpha
      movq [rdx], xmm0
      mov byte [rdx+3], 0xFF
      mov byte [rdx+7], 0xFF
      add rsi, 8
      add rdi, 8
      add rdx, 8
      ; esto es un poco más rápido (10%) que sumar en un registro
      ; extra y acceder con [ ] a la posición de memoria con
      ; desplazamiento
      add r9, 2                 ; procesamos dos pixeles a la vez
      cmp r9, r8
      jnz .columnas
    inc r10
    cmp r10, rcx
    jnz .filas

  pop rbp
  ret
