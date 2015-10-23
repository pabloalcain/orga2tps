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

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
void** PRIMER_PAG_LIBRE = (void**)0x100000;
void** ULTIMA_PAG_LIBRE = (void**)0x3ff000;

void mmu_inicializar() {
	(*PRIMER_PAG_LIBRE) = (void*)0x101000;
	(*ULTIMA_PAG_LIBRE) = (void*)0x3fe000;
}

uint mmu_proxima_pagina_fisica_libre(){
	uint PROX_PAG_LIBRE = (uint)0x100000;
	uint dir_pag_nueva = *ULTIMA_PAG_LIBRE;
	*ULTIMA_PAG_LIBRE = (void*)((*ULTIMA_PAG_LIBRE)-0x1000);		
	return dir_pag_nueva;
}

void mmu_mapear_areas_de_kernel_y_libre(unsigned int cr3){
	int i = 0;
	for(; i < 0x400; i++){
		mmu_mapear_pagina(i*0x1000, cr3, i*0x1000);
	}
}

void mmu_mapear_pagina  (uint virtual, uint cr3, uint fisica, uint attrs) {
	
	unsigned int index_page_dir = (virtual >> 22) & 0x3FF;
	unsigned int index_page_table = (virtual << 10);
	index_page_table = index_page_table >> 22;

	page_directory_entry *pd = (page_directory_entry *) cr3;
	page_directory_entry *entry_page_dir = &(pd[index_page_dir]);

	page_table_entry *dir_page_table;
	if (entry_page_dir->present) {
		dir_page_table = (page_table_entry *)(entry_page_dir->base_dir << 12);
	} else {
		dir_page_table = (page_table_entry *) mmu_proxima_pagina_fisica_libre();
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

//uint mmu_unmapear_pagina(uint virtual, uint cr3);

uint mmu_inicializar_dir_kernel() {
	int i;
	page_directory_entry *page_directory_table = (page_directory_entry *) PAGE_DIR;
	for (i = 0; i < 1024; i++) {
		page_directory_table[i].present = 0;
		page_directory_table[i].rw = 1;
		page_directory_table[i].user_supervisor = 0;
		page_directory_table[i].page_level_write_through = 0;
		page_directory_table[i].page_level_cache_disable = 0;
		page_directory_table[i].accessed = 0;
		page_directory_table[i].ignored = 0;
		page_directory_table[i].page_size = 0;
		page_directory_table[i].global = 0;
		page_directory_table[i].disp = 0;
		page_directory_table[i].base_dir = 0;
	}
	unsigned int cr3 = (unsigned int) page_directory_table; 

	page_directory_table[0].present = 1;
	page_directory_table[0].base_dir = PAGE_TABLE >> 12;

	for (i = 0; i < 1024; i++) {
		mmu_mapear_pagina(0x1000 * i, cr3, 0x1000 * i, 0x3);
	}

	return PAGE_DIR;
}

uint mmu_inicializar_memoria_perro(perro_t *perro, int index_jugador, int index_tipo) {
	uint dir_virtual_perro;
	uint dir_fisica_perro;
	uint attrs = 0x5; // U/S = 1 (User); R/W = 0 (Read only); P = 1 (present) 	
	cr3 = mmu_proxima_pagina_fisica_libre(); // Nuevo page directory para este perro
	mmu_mapear_areas_de_kernel_y_libre(cr3);
	if (index_jugador == 0) {
		// calculo de la dirección inicial de la cucha 
		dir_virtual_perro = 0x800000 + (VIDEO_COLS + 2) * 0x1000 // posicion en el mapa del perro del jugador 0

		dir_fisica_perro = 0x500000 + (VIDEO_COLS + 2) * 0x1000 // posicion en el mapa del perro del jugador 0


		
		mmu_mapear_pagina(dir_virtual_perro, cr3, dir_fisica_perro, atrrs)
	} else {
		
		// calculo de la dirección inicial de la cucha 
		dir_virtual_perro = 0x800000 + ((VIDEO_COLS * (VIDEO_FILS - 1 )) - 2) * 0x1000 // posicion en el mapa del perro del jugador 1

		dir_fisica_perro = 0x500000 + ((VIDEO_COLS * (VIDEO_FILS - 1 )) - 2) * 0x1000  // posicion en el mapa del perro del jugador 1
		
		mmu_mapear_pagina(dir_virtual_perro, cr3, dir_fisica_perro, atrrs);
	}

}