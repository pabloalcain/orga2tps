/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"
//#include "gdt.h"
//#include "i386.h"

tss tss_inicial;
tss tss_idle;

tss tss_jugadorA[MAX_CANT_PERROS_VIVOS];
tss tss_jugadorB[MAX_CANT_PERROS_VIVOS];

void tss_inicializar_tarea_inicial() {
  
    gdt[GDT_IDX_TSS_TAREA_INICIAL].base_0_15 = (((unsigned int) &tss_inicial) & 0xffff);
    gdt[GDT_IDX_TSS_TAREA_INICIAL].base_23_16 = (( (unsigned int) &tss_inicial) >> 16) & 0xff;
    gdt[GDT_IDX_TSS_TAREA_INICIAL].base_31_24 = (( (unsigned int) &tss_inicial) >> 24) & 0xff;
    gdt[GDT_IDX_TSS_TAREA_INICIAL].limit_0_15 = (unsigned short) sizeof(tss);
}

void tss_initializar_tarea_idle()
{
	tss_idle = (tss){
        0,                       // unsigned short  ptl;
        0,                       // unsigned short  unused0;
        KERNEL_STACK_DIR,       // unsigned int    esp0;
        GDT_IDX_DATA_LVL0 * 8,             // unsigned short  ss0;
        0,                       // unsigned short  unused1;
        0,                       // unsigned int    esp1;
        0,                       // unsigned short  ss1;
        0,                       // unsigned short  unused2;
        0,                       // unsigned int    esp2;
        0,                       // unsigned short  ss2;
        0,                       // unsigned short  unused3;
        PAGE_DIR,           // unsigned int    cr3;
        COD_TAREA_IDLE_DIR,          // unsigned int    eip;
        0x202,                   // unsigned int    eflags;
        0,                       // unsigned int    eax;
        0,                       // unsigned int    ecx;
        0,                       // unsigned int    edx;
        0,                       // unsigned int    ebx; 
        KERNEL_STACK_DIR,       // unsigned int    esp;    
        KERNEL_STACK_DIR,       // unsigned int    ebp;
        0,                       // unsigned int    esi;
        0,                       // unsigned int    edi;
        GDT_IDX_DATA_LVL0 * 8,             // unsigned short  es;
        0,                       // unsigned short  unused4;
        GDT_IDX_COD_LVL0  * 8,             // unsigned short  cs;
        0,                       // unsigned short  unused5;
        GDT_IDX_DATA_LVL0 * 8,             // unsigned short  ss;
        0,                       // unsigned short  unused6;
        GDT_IDX_DATA_LVL0 * 8,             // unsigned short  ds;
        0,                       // unsigned short  unused7;
        GDT_IDX_DATA_LVL0 * 8,             // unsigned short  fs;
        0,                       // unsigned short  unused8;
        GDT_IDX_DATA_LVL0 * 8,             // unsigned short  gs;
        0,                       // unsigned short  unused9;
        0,                       // unsigned short  ldt;
        0,                       // unsigned short  unused10;
        0,                       // unsigned short  dtrap;
        0xFFFF                   // unsigned short  iomap;
    };
}


void tss_inicializar_tarea_perro(unsigned int jugador, 
                            unsigned int numero_tarea,
                            unsigned int task_gdt_index,
                            unsigned int cr3) // recibo como parametro el cr3 cuando llamo a mmu_inicializar_perro
{
    int indice_tss = numero_tarea;

	if (jugador == JUGADOR_A) {

		tss_jugadorA[indice_tss].cr3 = cr3; 			
	    tss_jugadorA[indice_tss].cs = (0xA << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorA[indice_tss].ds = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorA[indice_tss].ss = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorA[indice_tss].fs = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorA[indice_tss].gs = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorA[indice_tss].es = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorA[indice_tss].iomap = 0xFFFF;
	    
	    tss_jugadorA[indice_tss].ss0 = 0x10;
	    
	    tss_jugadorA[indice_tss].eflags = 0x202;

		tss_jugadorA[indice_tss].eax = 0x0;
		tss_jugadorA[indice_tss].ecx = 0x0;
		tss_jugadorA[indice_tss].edx = 0x0;
		tss_jugadorA[indice_tss].ebx = 0x0;
		
		
		tss_jugadorA[indice_tss].esi = 0x0;
		tss_jugadorA[indice_tss].edi = 0x0;

	    tss_jugadorA[indice_tss].eip = 0x401000; // direccion virtual del codigo de la tarea 
	    tss_jugadorA[indice_tss].ebp = 0x00402000 - 12;  // direccion 401000 + 1000 - 2 porque van los parametros que dice en el enunciado
	    tss_jugadorA[indice_tss].esp = 0x00402000 - 12;
	    tss_jugadorA[indice_tss].esp0 = mmu_proxima_pagina_fisica_libre() + 0x1000; // ultima direccion de la pagina, la pila crece a direcciones inferiores
	    tss_jugadorA[indice_tss].dtrap = 0x0; // para indicar que no tiene que ser una interrupcion de tipo trap 


	    
	    gdt[task_gdt_index].base_0_15 	= (( (unsigned int) &tss_jugadorA[indice_tss]) & 0xffff);
	    gdt[task_gdt_index].base_23_16	= (( (unsigned int) &tss_jugadorA[indice_tss]) >> 16) & 0xff;
	    gdt[task_gdt_index].base_31_24 	= (( (unsigned int) &tss_jugadorA[indice_tss]) >> 24) & 0xff;
	    
	}  else {
		tss_jugadorB[indice_tss].cr3 = cr3; 			
	    tss_jugadorB[indice_tss].cs = (0xA << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorB[indice_tss].ds = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorB[indice_tss].ss = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorB[indice_tss].fs = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorB[indice_tss].gs = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorB[indice_tss].es = (0xB << 3) | 0x3;   // segmento y permiso de nivel usuario
	    tss_jugadorB[indice_tss].iomap = 0xFFFF;
	    
	    tss_jugadorB[indice_tss].ss0 = 0x10;
	    
		tss_jugadorB[indice_tss].eax = 0x0;
		tss_jugadorB[indice_tss].ecx = 0x0;
		tss_jugadorB[indice_tss].edx = 0x0;
		tss_jugadorB[indice_tss].ebx = 0x0;
		
		
		tss_jugadorB[indice_tss].esi = 0x0;
		tss_jugadorB[indice_tss].edi = 0x0;	    

	    tss_jugadorB[indice_tss].eflags = 0x202;
	    tss_jugadorB[indice_tss].eip = 0x401000; // direccion virtual del codigo de la tarea 
	    tss_jugadorB[indice_tss].ebp = 0x00402000 - 12;  // direccion 401000 + 1000 - 2 porque van los parametros que dice en el enunciado
	    tss_jugadorB[indice_tss].esp = 0x00402000 - 12;
	    tss_jugadorB[indice_tss].esp0 = mmu_proxima_pagina_fisica_libre() + 0x1000; // ultima direccion de la pagina, la pila crece a direcciones inferiores
	    tss_jugadorB[indice_tss].dtrap = 0x0; // para indicar que no tiene que ser una interrupcion de tipo trap 
	    
	    gdt[task_gdt_index].base_0_15 	= (( (unsigned int) &tss_jugadorB[indice_tss]) & 0xffff);
	    gdt[task_gdt_index].base_23_16	= (( (unsigned int) &tss_jugadorB[indice_tss]) >> 16) & 0xff;
	    gdt[task_gdt_index].base_31_24 	= (( (unsigned int) &tss_jugadorB[indice_tss]) >> 24) & 0xff;
	    
	}  
}

void tss_inicializar_gdt_perros(){
    int i;
    for (i = 0; i < 8; i++){
        /* Inicializar segmentos de perros A*/
        gdt_agregarSegmento(
            (GDT_IDX_TSS_BASE_PERROS_A + i),
            (int) &tss_jugadorA[i],
            sizeof(tss),
            9,
            0,
            0,
            1,
            0,
            0,
            0
            );
        /* Inicializar segmentos de perros B*/
        gdt_agregarSegmento(
            (GDT_IDX_TSS_BASE_PERROS_B + i),
            (int) &tss_jugadorB[i],
            sizeof(tss),
            9,
            0,
            0,
            1,
            0,
            0,
            0
            );
    }
}

void tss_inicializar()
{
//	breakpoint();
    gdt_agregarSegmento(
        GDT_IDX_TSS_TAREA_INICIAL,/* indice */
        (int)&tss_inicial,       /* base */
        sizeof(tss_inicial) - 1, /* limite */
        9,                       /* tipo */
        0,                       /* s */
        0,                       /* dpl */
        1,                       /* p */
        0,                       /* l */
        0,                       /* db */
        0                        /* g */
    );
  //  breakpoint();
    gdt_agregarSegmento(
        GDT_IDX_TSS_TAREA_IDLE,   /* indice */
        (int)&tss_idle,          /* base */
        sizeof(tss_idle) - 1,    /* limite */
        9,                       /* tipo */
        0,                       /* s */
        0,                       /* dpl */
        1,                       /* p */
        0,                       /* l */
        0,                       /* db */
        0                        /* g */ //modificado
    );
   // breakpoint();
    tss_inicializar_tarea_inicial();
  //  breakpoint();
    tss_initializar_tarea_idle();
   // breakpoint();
}
