; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "imprimir.mac"
extern GDT_DESC 
extern IDT_DESC 
extern screen_inicializar
extern idt_inicializar

extern mmu_inicializar
extern mmu_inicializar_dir_kernel

extern deshabilitar_pic
extern resetear_pic
extern habilitar_pic 
extern game_inicializar
extern mmu_unmapear_pagina
extern tss_inicializar


global start


;; Saltear seccion de datos
jmp start

;;
;; start de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0


    ; Habilitar A20
    call habilitar_A20

    ; Cargar la GDT
    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Saltar a modo protegido
    jmp (0x8*8):modo_protegido

    modo_protegido: 
    BITS 32

    ; Establecer selectores de segmentos

    mov ax, (0x9*8) ; {index: 9 | gdt/ldt: 0 | rpl: 0 }

    mov ds, ax
    mov es, ax
    mov gs, ax
    mov ss, ax

    mov ax, (0xC*8) ; {index: 12 | gdt/ldt: 0 | rpl: 0 }
    mov fs, ax

    ; Establecer la base de la pila
    mov ebp, 0x27000
    mov esp, ebp

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mp iniciando_mp_msg, iniciando_mp_len, 0x07, 0, 0

    ; Inicializar el juego
    call game_inicializar

    ; Inicializar pantalla
    xor ebx, ebx
    xor ax, ax
    limpiar_pantalla:
        mov [fs:ebx*2], ax
        inc ebx
        cmp ebx, 2000
        jb limpiar_pantalla

    call screen_inicializar

    ; Inicializar el manejador de memoria
    call mmu_inicializar

    ; Inicializar el directorio de paginas
    call mmu_inicializar_dir_kernel ;eax = 0x27000

    ; Cargar directorio de paginas
    mov cr3, eax     ;cr3[12:31]=0x27,cr3[0:11]=0x0 => cr3 = 0010 0111 0000 0000 0000 == 0x27000

    ; Habilitar paginacion
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

    ; Inicializar tss

    xchg bx, bx
    ; Inicializar tss de la tarea Idle
    call tss_inicializar

    ; Inicializar el scheduler

    ; Inicializar la IDT
    call idt_inicializar


    ; Cargar IDT
    lidt [IDT_DESC]


    ; Configurar controlador de interrupciones
    call deshabilitar_pic
    call resetear_pic
    call habilitar_pic

    ; ; Habilitar interrupciones
    sti    

    ; Cargar tarea inicial
    ; (se hace en game_inicializar() )
    
    ; EJERCICIO 2: Generar interrupcion
    ; mov eax, 1 
    ; mov edx, 0
    ; div edx

    ; EJERCICIO 4) f): Desmapear la ultima pag del kernel
    ; mov eax,[0x3FF000]
    ; mov eax, 0x27000
    ; push eax
    ; mov eax, 0x3FF000
    ; push eax
    ; call mmu_unmapear_pagina
    ; pop eax
    ; pop eax
    ; mov eax,[0x3FF000]

    ; Saltar a la primera tarea: Idle
    jmp (0x14*8):0          ; salto a tarea IDLE, 14 = GDT_IDX_TSS_TAREA_IDLE

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
