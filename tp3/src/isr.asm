; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_atender_tick
extern sched_tarea_actual
extern sched_atender_tick
extern game_atender_teclado
extern get_cant_perros_activos


;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
MSG_ISR0  db 'Divide Error',0
MSG_ISR1  db 'RESERVED',0
MSG_ISR2  db 'NMI Interrupt',0
MSG_ISR3  db 'Breakpoint',0
MSG_ISR4  db 'Overflow',0
MSG_ISR5  db 'BOUND Range Exceeded',0
MSG_ISR6  db 'Invalid Opcode (Undefined Opcode)',0
MSG_ISR7  db 'Device Not Available',0
MSG_ISR8  db 'Double Fault',0
MSG_ISR9  db 'Coprocessor Segment Overrun',0
MSG_ISR10 db 'Invalid TSS',0
MSG_ISR11 db 'Segment Not Present',0
MSG_ISR12 db 'Stack-Segment Fault',0
MSG_ISR13 db 'General Protection Exception',0
MSG_ISR14 db 'Page Fault',0
MSG_ISR15 db 'Reserved',0
MSG_ISR16 db 'x87 FPU Error',0
MSG_ISR17 db 'Alignment Check',0
MSG_ISR18 db 'Machine Check',0
MSG_ISR19 db 'SIMD FP Exception',0

%macro ISR 3

global _isr%1

_isr%1:
    mov eax, %1
	imprimir_texto_mp %2, %3, 0x07, 0, 0
    jmp $

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler
;;
isrnumero1:
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
isrnumero2:
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
                     dd 0x00000000
isrnumero:           dd 0x00000000
isrClock:            db '|/-\'
;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0, MSG_ISR0, 12
ISR 1, MSG_ISR1, 8
ISR 2, MSG_ISR2, 13
ISR 3, MSG_ISR3, 10
ISR 4, MSG_ISR4, 8
ISR 5, MSG_ISR5, 20
ISR 6, MSG_ISR6, 33
ISR 7, MSG_ISR7, 20
ISR 8, MSG_ISR8, 12
ISR 9, MSG_ISR9, 27
ISR 10, MSG_ISR10, 11
ISR 11, MSG_ISR11, 19
ISR 12, MSG_ISR12, 19
ISR 13, MSG_ISR13, 28
ISR 14, MSG_ISR14, 10
ISR 15, MSG_ISR15, 8
ISR 16, MSG_ISR16, 13
ISR 17, MSG_ISR17, 15
ISR 18, MSG_ISR18, 13
ISR 19, MSG_ISR19, 17


;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
    cli
    pushad
    pushfd

    call fin_intr_pic1  

    call sched_proxima_a_ejecutar
    ; ax tiene el indice en la gdt
    mov bx, ax
    str ax

    mov word [sched_tarea_selector], ax


    cmp bx, ax
    je .fin

    .fin:
    popfd
    popad
    sti
    iret
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
    cli
    pushad
    pushfd
    call fin_intr_pic1

    in al, 0x60
    push eax
    call game_atender_teclado
    pop eax

fin33:
    popfd
    popad
    sti
    iret

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr0x46
_isr0x46:
    cli
    
    mov eax, 0x42
    xchg bx, bx
    sti
    iret
