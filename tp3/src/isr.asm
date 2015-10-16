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
    jmp $

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

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

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


