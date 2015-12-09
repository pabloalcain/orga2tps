
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

#define IDT_ENTRY(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x0040;                                                                \
    idt[numero].attr = (unsigned short) 0x8e00;                                                                  \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF); \


void idt_inicializar() {
    IDT_ENTRY(0);
    IDT_ENTRY(1);
    IDT_ENTRY(2);
    IDT_ENTRY(3);
    IDT_ENTRY(4);
    IDT_ENTRY(5);
    IDT_ENTRY(6);
    IDT_ENTRY(7);
    IDT_ENTRY(8);
    IDT_ENTRY(9);
    IDT_ENTRY(10);
    IDT_ENTRY(11);
    IDT_ENTRY(12);
    IDT_ENTRY(13);
    IDT_ENTRY(14);
    IDT_ENTRY(15);
    IDT_ENTRY(16);
    IDT_ENTRY(17);
    IDT_ENTRY(18);
    IDT_ENTRY(19);
    
    //RELOJ
    IDT_ENTRY(32);
    //TECLADO
    IDT_ENTRY(33);
    //INTERRUPCION
    //IDT_ENTRY(70); 
    
    idt[0x46].offset_0_15 = (unsigned short) ((unsigned int)(&_isr0x46) & (unsigned int) 0xFFFF);        
        idt[0x46].segsel = (unsigned short) 0x0040;                                                                  
        idt[0x46].attr = (unsigned short) 0xee00;                                                                  
        idt[0x46].offset_16_31 = (unsigned short) ((unsigned int)(&_isr0x46) >> 16 & (unsigned int) 0xFFFF);

}
