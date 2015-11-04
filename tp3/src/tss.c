/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"
#include "gdt.h"
#include "i386.h"

tss tss_inicial;
tss tss_idle;

tss tss_jugadorA[MAX_CANT_PERROS_VIVOS];
tss tss_jugadorB[MAX_CANT_PERROS_VIVOS];

void tss_inicializar_tarea_inicial() {
	gdt[GDT_IDX_TAREA_INICIAL].base_0_15 = (((unsigned int) &	tss_inicial) & 0xffff);
    gdt[GDT_IDX_TAREA_INICIAL].base_23_16 = (( (unsigned int) &	tss_inicial) >> 16) & 0xff;
    gdt[GDT_IDX_TAREA_INICIAL].base_31_24 = (( (unsigned int) &	tss_inicial) >> 24) & 0xff;
    gdt[GDT_IDX_TAREA_INICIAL].limit_0_15 = (unsigned short) sizeof(tss);
}

void tss_initializar_tarea_idle()
{
	tarea_idle.cr3 = (unsigned int) PAGE_DIR; 			
    tarea_idle.cs = GDT_IDX_COD_LVL0;
    tarea_idle.ds = GDT_IDX_DATA_LVL0;
    tarea_idle.ss = GDT_IDX_DATA_LVL0;
    tarea_idle.fs = GDT_IDX_DATA_LVL0;
    tarea_idle.gs = GDT_IDX_DATA_LVL0;
    tarea_idle.iomap = 0xFFFF;
    tarea_idle.eflags = 0x202;
    tarea_idle.eip = 0x16000; 
    tarea_idle.ebp = KERNEL_STACK_DIR;
    tarea_idle.esp = KERNEL_STACK_DIR;
    
    gdt[TAREA_IDLE_GDT_INDEX].base_0_15 	= (( (unsigned int) & tarea_idle) & 0xffff);
    gdt[TAREA_IDLE_GDT_INDEX].base_23_16	= (( (unsigned int) & tarea_idle) >> 16) & 0xff;
    gdt[TAREA_IDLE_GDT_INDEX].base_31_24 	= (( (unsigned int) & tarea_idle) >> 24) & 0xff;
    gdt[TAREA_IDLE_GDT_INDEX].limit_0_15 	= (unsigned short) sizeof(tss);
}


void tss_inicializar_tarea_perro(unsigned int jugador, 
                            unsigned int numero_tarea
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
	    tss_jugadorA[indice_tss].eip = 0x401000; // direccion virtual del codigo de la tarea 
	    tss_jugadorA[indice_tss].ebp = ;  // direccion 401000 + 1000 - 2 porque van los parametros que dice en el enunciado
	    tss_jugadorA[indice_tss].esp = ;
	    tss_jugadorA[indice_tss].esp0 = ;
	    
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
	    
	    tss_jugadorB[indice_tss].eflags = 0x202;
	    tss_jugadorB[indice_tss].eip = 0x401000; // direccion virtual del codigo de la tarea 
	    tss_jugadorB[indice_tss].ebp = ;  // direccion 401000 + 1000 - 2 porque van los parametros que dice en el enunciado
	    tss_jugadorB[indice_tss].esp = ;
	    tss_jugadorB[indice_tss].esp0 = ;
	    
	    gdt[task_gdt_index].base_0_15 	= (( (unsigned int) &tss_jugadorB[indice_tss]) & 0xffff);
	    gdt[task_gdt_index].base_23_16	= (( (unsigned int) &tss_jugadorB[indice_tss]) >> 16) & 0xff;
	    gdt[task_gdt_index].base_31_24 	= (( (unsigned int) &tss_jugadorB[indice_tss]) >> 24) & 0xff;
	    
	}  
}


void tss_inicializar()
{
	tss_initializar_tarea_inicial();
    tss_initializar_tarea_idle();
    
    tss_inicializar_tarea_perro(0, );

    
}