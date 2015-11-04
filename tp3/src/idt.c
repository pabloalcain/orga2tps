
/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"

#include "tss.h"

idt_entry idt[255] = { };

idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0, 0);
        ...
        IDT_ENTRY(19, 0);

        ...
    }
*/

// descomentar para usar, seteando segsel y attr segun corresponda

#define IDT_ENTRY(numero, dpl)                                                                                   \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) (0x8*8);                                                                  \
    idt[numero].attr = (unsigned short) 0x8E00 | (((unsigned short)(dpl & 0x3)) << 13);                          \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

/*Ejercicio 5*/
#define IDT_ENTRY2(numero, dpl) \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF); \
    idt[numero].segsel = (unsigned short) 0x08; \
    idt[numero].attr = (unsigned short) 0xEE00 | (((unsigned short)(dpl & 0x3)) << 13); \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

void idt_inicializar() {
    // Excepciones
    IDT_ENTRY(0, 00);
    IDT_ENTRY(1, 00);
    IDT_ENTRY(2, 00);
    IDT_ENTRY(3, 00);
    IDT_ENTRY(4, 00);
    IDT_ENTRY(5, 00);
    IDT_ENTRY(6, 00);
    IDT_ENTRY(7, 00);
    IDT_ENTRY(8, 00);
    IDT_ENTRY(9, 00);
    IDT_ENTRY(10, 00);
    IDT_ENTRY(11, 00);
    IDT_ENTRY(12, 00);
    IDT_ENTRY(13, 00);
    IDT_ENTRY(14, 00);
    IDT_ENTRY(15, 00);
    IDT_ENTRY(16, 00);
    IDT_ENTRY(17, 00);
    IDT_ENTRY(18, 00);
    IDT_ENTRY(19, 00);
/*Ejercicio 5*/
    IDT_ENTRY(32, 00); // Reloj
    IDT_ENTRY(33, 00); // Teclado
    IDT_ENTRY2(0x46, 0x3);
	// llamar a IDT_ENTRY una vez por cada entrada a configurar (es una macro que no soporta loops)
}
