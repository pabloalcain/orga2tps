/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */
#define ADDR_COD_TAREA_A1 		0x10000
#define ADDR_COD_TAREA_A2 		0x11000
#define ADDR_COD_TAREA_B1 		0x12000
#define ADDR_COD_TAREA_B2 		0x13000
#define ADDR_COD_TAREA_IDLE 	0x16000
#define SIZE_W					78
#define SIZE_H					44

#define EL_MAPA_VIRTUAL                   0x8000000
#define EL_MAPA_FISICA                    0x0500000


/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
void** PRIMER_PAG_LIBRE = (void**)0x100000;
void** ULTIMA_PAG_LIBRE = (void**)0x3ff000;

void mmu_inicializar() {
	(*PRIMER_PAG_LIBRE) = (void*)0x101000;
	(*ULTIMA_PAG_LIBRE) = (void*)0x3fe000;
}


void mmu_mapear_pagina_jugador_A(unsigned int cr3, unsigned int pos_perro, unsigned int offset_y){
	mmu_mapear_pagina((unsigned int)EL_MAPA_VIRTUAL, cr3, pos_perro);
}

void mmu_mapear_pagina_jugador_B(unsigned int cr3, unsigned int pos_perro, unsigned int offset_y){
	mmu_mapear_pagina((unsigned int)EL_MAPA_VIRTUAL, cr3, pos_perro);
}

void mmu_mapear_areas_de_kernel_y_libre(unsigned int cr3){
	int i = 0;
	for(; i < 0x400; i++){
		mmu_mapear_pagina(i*0x1000, cr3, i*0x1000);
	}
}

void mmu_mapear_con_identity_mapping(){
	page_table_entry *pt = (page_table_entry*)PAGE_TABLE;
	int i;
	unsigned int page_dir_base = 0x0;
	// (1024 * 4KB) - 1B = 4MB - 1 = 0x3FFFFF
	for (i = 0; i < 1024; i++) {
	    pt->base_dir = page_dir_base;
	    pt->disp = 0;
	    pt->global = 0;
	    pt->page_table_attr_index = 0;
	    pt->dirty_bit = 0;
	    pt->accessed = 0;
	    pt->page_level_cache_disable = 0;
	    pt->page_level_write_through = 0;
	    pt->user_supervisor = 0;
	    pt->rw = 1;
	    pt->present = 1;
		page_dir_base += 1;
		pt += 1; // avanzar a la siguiente entrada
	}
}

unsigned int mmu_inicializar_dir_kernel(){
	page_directory_entry *pd = (page_directory_entry*)PAGE_DIR;
	int i;
	for (i = 0; i < 1024; i++){
		pd->present = 0;
	    pd->rw = 0;
	    pd->user_supervisor = 0;
	    pd->page_level_write_through = 0;
	    pd->page_level_cache_disable = 0;
	    pd->accessed = 0;
	    pd->ignored = 0;
	    pd->page_size = 0;
	    pd->global = 0;
	    pd->disp = 0;
	    pd->base_dir = 0;

		pd += 1;	// avanzar a la siguiente entrada
	}
	pd = (page_directory_entry *)PAGE_DIR;
	pd->base_dir = PAGE_TABLE >> 12; // supervisor, read/write, present
    pd->rw = 1;
    pd->present = 1;
	mmu_mapear_con_identity_mapping();
	return PAGE_DIR;
}

void mmu_mapear_pagina(
	unsigned int virtual,
	unsigned int cr3, 
	unsigned int fisica) {
	
	unsigned int index_page_dir = (virtual >> 22) & 0x3FF;
	unsigned int index_page_table = (virtual << 10);
	index_page_table = index_page_table >> 22;

	page_directory_entry *pd = (page_directory_entry *) cr3;
	page_directory_entry *entry_page_dir = &(pd[index_page_dir]);

	page_table_entry *dir_page_table;
	if (entry_page_dir->present) {
		dir_page_table = (page_table_entry *)(entry_page_dir->base_dir << 12);
	} else {
		dir_page_table = (page_table_entry *) mmu_solicitar_pagina_nueva();
		entry_page_dir->base_dir = ((unsigned int)dir_page_table) >> 12;
		entry_page_dir->present = 1;
		entry_page_dir->rw = 1;
		if(cr3 == PAGE_DIR || virtual <0x8000000)
			entry_page_dir->user_supervisor = 0;
		else
			entry_page_dir->user_supervisor = 1;
	}

	page_table_entry *entry_page_table = &(dir_page_table[index_page_table]);
	entry_page_table->base_dir = fisica >> 12;
	entry_page_table->present = 1;
	entry_page_table->rw = 1;
	if(cr3 == PAGE_DIR || virtual <0x8000000)
		entry_page_table->user_supervisor = 0;
	else
		entry_page_table->user_supervisor = 1;
	tlbflush(); /* Invalidar cache de traduccion de direcciones */
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3){
	unsigned int index_page_dir = (virtual >> 22) & 0x3FF;
	unsigned int index_page_table = (virtual << 10);
	index_page_table = index_page_table >> 22;

	page_directory_entry *pd = (page_directory_entry *) cr3;
	page_directory_entry *entry_page_dir = &(pd[index_page_dir]);     /* Ir a la entrada dentro del page directory*/

	if (! entry_page_dir->present) {
		return;
	}

	page_table_entry *pt = (page_table_entry *) (entry_page_dir->base_dir >> 12);
	page_table_entry *entry_page_table = &(pt[index_page_table]);
	if (entry_page_table->present) {
		/* Si la entrada buscada estÃ¡ en el page directory */
		entry_page_table->present = 0;
		tlbflush();	
	}
}

void copiar_codigo(unsigned int* codigo, unsigned int* dst, unsigned int cr3){
	unsigned int cr3_anterior = rcr3();
	lcr3(cr3);
	int i;
	for (i = 0; i < 1024; i++) {
		dst[i] = codigo[i];
	}
	lcr3(cr3_anterior);
}

/* Funcion para inicializar un perro en particular*/
unsigned int mmu_inicializar_memoria_perro(unsigned int jugador) {
	unsigned int cr3 = (unsigned int)mmu_solicitar_cr3_nuevo(jugador);
	mmu_mapear_areas_de_kernel_y_libre(cr3);
	unsigned int pos_perro;
	unsigned int cod_perro_addr;
	if (jugador == JUGADOR_A
	) {
		pos_perro = (EL_MAPA_FISICA + (0x1000 * SIZE_W * (jugadorA.x-1)) + 0x1000);
		mmu_mapear_pagina_jugador_A(cr3, pos_perro, (jugadorA.x)-1);
		mmu_mapear_pagina((unsigned int)pos_perro, PAGE_DIR, pos_perro);
		if (jugadorA.tipo_perro_a_lanzar == TIPO_1) {
			cod_perro_addr =  ADDR_COD_TAREA_A1;
		} else {
			cod_perro_addr =  ADDR_COD_TAREA_A2;
		}
	} else {
		pos_perro = (EL_MAPA_FISICA + (0x1000 * SIZE_W * (jugadorB.x-1)) + 0x1000 * SIZE_W - 0x2000);
		mmu_mapear_pagina_jugador_B(cr3, pos_perro, (jugadorB.x)-1);
		mmu_mapear_pagina((unsigned int)pos_perro, PAGE_DIR, pos_perro);
		if (jugadorB.tipo_perro_a_lanzar == TIPO_1) {
			cod_perro_addr =  ADDR_COD_TAREA_B1;
		} else {
			cod_perro_addr =  ADDR_COD_TAREA_B2;
		}
	}
	copiar_codigo((unsigned int*)cod_perro_addr, (unsigned int*)pos_perro, 0x27000);
	return cr3;
}

/*Reserva una pÃ¡gina en blanco del Ãrea de PÃ¡ginas de AsignaciÃ³n DinÃ¡mica, y devuelve su direcciÃ³n*/
void* mmu_solicitar_pagina_nueva(){
	void* dir_pag_nueva = *ULTIMA_PAG_LIBRE;
	*ULTIMA_PAG_LIBRE = (void*)((*ULTIMA_PAG_LIBRE)-0x1000);		
	return dir_pag_nueva;
}

void* mmu_solicitar_cr3_nuevo(unsigned int jugador){
	void* dir_pag_nueva = *PRIMER_PAG_LIBRE;
	if(jugador == JUGADOR_B) dir_pag_nueva += 0x20000;
	*PRIMER_PAG_LIBRE = (void*)((*PRIMER_PAG_LIBRE)+0x1000);		
	return dir_pag_nueva;
}