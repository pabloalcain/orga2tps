default rel
global _blur_asm
global blur_asm
extern alloc_creafiltro
extern free

section .data
mask_rot: DB 1, 2, 0, 0xFF, 5, 6, 4, 0xFF
mask_dw2b: DB 0, 4, 8, 12

section .text
;void blur_asm (unsigned char *src, unsigned char *dst,
;              (       RDI        ,         RSI       ,
;               int filas, int cols, float sigma, int radius)
;                  RDX   ,   RCX   ,    XMM0    ,     R8    )
_blur_asm:
blur_asm:
  push rbp
  mov rbp, rsp
  push r15

  push rdi
  push rsi
  push rdx
  push rcx
  push r8
  mov rdi, r8
  call alloc_creafiltro
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi

  mov r15, rax
  




  ; esto es para el tonto filtro de promedio
  cvtsi2ss xmm4, r8
  shufps xmm4, xmm4, 0x00
  addps xmm4, xmm4
  mov rax, 1
  cvtsi2ss xmm5, rax
  shufps xmm5, xmm5, 0x00
  addps xmm4, xmm5
  mulps xmm4, xmm4
  movd xmm5, [mask_dw2b]
  ; rax apunta a la pos relativa del primer elemento del cuadrado de
  ; vecinos

  mov r9, r8
  xor rax, rax
  .zero:
    sub rax, rcx
    ; nos movemos r8 filas para arriba
    sub r9, 1
  jnz .zero
  sub rax, r8                   ; r8 columnas a la izquierda
  sal rax, 2                    ; 4 bytes/casillero
  
  ; r13 es la pos relativa entre de (i, 2*r+1) y (i+1, 0) en el
  ; cuadrado de vecinos: le restamos dos veces el radio + 1 a una fila

  mov r13, rcx
  sub r13, r8
  sub r13, r8
  sub r13, 1
  sal r13, 2                    ; 4 bytes/casillero
    
  ;rcx y rdx son el último casillero que filtramos
  sub rcx, r8
  sub rdx, r8

  ;rdi y rsi apuntan al primer filtrado
  sub rdi, rax
  sub rsi, rax
  
  mov r9, r8
  .filas:
    mov r10, r8
    .columnas:
      pxor xmm1, xmm1
      pmovzxbd xmm3, xmm1
      cvtdq2ps xmm1, xmm3
      mulps xmm1, xmm4
      mov r14, rdi
      add r14, rax
      mov r11, r8
      not r11
      xor rbx, rbx
      .neigh_filas:
        mov r12, r8
        not r12
        .neigh_cols:
          movd xmm2, [r14]
          pmovzxbd xmm3, xmm2
          cvtdq2ps xmm2, xmm3

          movd xmm4, [r15 + rbx]
          shufps xmm4, xmm4, 0x00

          mulps xmm2, xmm4
          addps xmm1, xmm2
          inc r12
          add r14, 4
          add rbx, 4
          cmp r12, r8
        jnz .neigh_cols
        add r14, r13
        inc r11
        cmp r11, r8
      jnz .neigh_filas

      cvtps2dq xmm3, xmm1
      pshufb xmm3, xmm5
      movd [rsi], xmm3
      add rdi, 4
      add rsi, 4
      inc r10
      cmp r10, rcx
    jnz .columnas
    mov r11, r8
    sal r11, 3
    add rdi, r11
    add rsi, r11
    inc r9
    cmp r9, rdx
  jnz .filas
  mov rdi, r15
  call free
  pop r15
  pop rbp

  ret
