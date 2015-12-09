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

uint pagLibre = 0x100000;
uint cantPagLibre = 768;
uint paginaJugadorA;
uint paginaJugadorB;


/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
void** PRIMER_PAG_LIBRE = (void**)0x100000;
void** ULTIMA_PAG_LIBRE = (void**)0x3ff000;

void mmu_inicializar() {
	(*PRIMER_PAG_LIBRE) = (void*)0x101000;
	(*ULTIMA_PAG_LIBRE) = (void*)0x3fe000;
}

void * siguiente_libre;

void * dar_siguiente()
{
    uint i;

    for(i = 0; i<1024; i++)
    {
        ((page_directory_entry *) siguiente_libre)[i].present = 0;
    }

    siguiente_libre += 0x1000;
    return siguiente_libre - 0x1000;
}


// void mmu_mapear_pagina_jugador_A(unsigned int cr3, unsigned int pos_perro, unsigned int offset_y){
// 	mmu_mapear_pagina((unsigned int)EL_MAPA_VIRTUAL, cr3, pos_perro);
// }

// void mmu_mapear_pagina_jugador_B(unsigned int cr3, unsigned int pos_perro, unsigned int offset_y){
// 	mmu_mapear_pagina((unsigned int)EL_MAPA_VIRTUAL, cr3, pos_perro);
// }

void mmu_mapear_areas_de_kernel_y_libre(unsigned int cr3){
	int i = 0;
	for(; i < 0x400; i++){
		mmu_mapear_pagina(i*0x1000, cr3, i*0x1000, 0x2);
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

void mmu_inicializar_pagina(uint * pagina){
	int i = 0;
	while(i<1024){
		*pagina = 0x00000000;
		pagina += 1;
		i++;
	}
	
}

void mmu_mapear_pagina(uint virtual, uint cr3, uint fisica, uint attrs){
	uint *pagDir = (uint *) ((cr3 & 0xFFFFF000) + ((virtual >> 22 )*4));

	uint *pageTable;
	uint *pageTableEntry;
	if ( *pagDir % 2 == 1){ 				// ESTA PRESENTE?
		
		
		pageTableEntry = (uint *) ((*pagDir & 0xFFFFF000) + ((virtual >> 12) & 0x000003FF)*4);
		uint pageDirAux = *pagDir;
		pageDirAux = pageDirAux & 0xFFFFFFFE;
		uint atr_aux = attrs;
		atr_aux = atr_aux >> 1;
		if (pageDirAux % 4 == 0 && atr_aux % 2 == 1){ 			// si r/w == 0 y attrs es de r/w
			*pagDir = *pagDir | 2;

		}
		atr_aux = atr_aux >> 1;		
		pageDirAux = pageDirAux & 0xFFFFFFFC;
		if (pageDirAux % 8 != 0 && atr_aux % 2 == 0){ 			// si u/s == 1 y attrs es de supervisor
			*pagDir = *pagDir & 0xFFFFFFFB;
		}
	} else {
		pageTable = (uint *) mmu_solicitar_pagina_nueva();
		*pagDir = ((uint )pageTable & 0xFFFFF000) | 0x00000007;			// LE PASAMOS LA DIRECCION DE LA TABLA Y EL PRESENTE
		
		pageTableEntry = pageTable + ((virtual >> 12) & 0x000003FF);
		mmu_inicializar_pagina(pageTable);
	}
	


	*pageTableEntry = 0x00000000 | fisica;
	*pageTableEntry = (*pageTableEntry & 0xFFFFF000) | attrs;

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

void copiar_codigo(unsigned int* codigo, unsigned int* dst){
	int i;
	for (i = 0; i < 1024; i++) {
		dst[i] = codigo[i];
	}
}


uint mmu_xy2fisica(uint x, uint y){
	return 0x500000+x*4096+y*4096*80;
}

uint mmu_xy2virtual(uint x, uint y){
	return 0x800000+x*4096+y*4096*80;
}


/* Funcion para inicializar un perro en particular*/
uint mmu_inicializar_memoria_perro(perro_t *perro, int index_jugador, int index_tipo){
	
	uint *pagDir = (uint *) mmu_solicitar_pagina_nueva(); 	// PIDO UNA PAGINA LIBRE

	mmu_inicializar_pagina(pagDir); 	// LIMPIO PAGINA
	
	int i = 0x00000000;
	while (i<1024){ 				
		mmu_mapear_pagina(i*4096,(uint )pagDir,i*4096,0x007);
		i++;
	}
	
	uint aCopiar;
	if (index_jugador == 0){
		if (perro->tipo == 0){ 			// TAREA A1
			aCopiar = 0x10000;
		} else {		 				// TAREA A2
			aCopiar = 0x11000;
		}
		mmu_mapear_pagina(0x400000, (uint) pagDir, paginaJugadorA, 0x007);

	} else{
		if (perro->tipo == 0){ 			// TAREA B1
			aCopiar = 0x12000;
		} else {				 		// TAREA B2
			aCopiar = 0x13000;
		}
		mmu_mapear_pagina(0x400000, (uint) pagDir, paginaJugadorB, 0x007);

	}
	
	
	int j = 0;						// ESTO NO DEBERIA ESTAR, PERO SI LO SACO DEJA DE ANDAR
	while (j<3520){ 				// MAPEO CON EL MAPA
		mmu_mapear_pagina(0x800000+j*4096,(uint )pagDir,j*4096+0x500000,0x007);
		j++;
	}

	mmu_mapear_pagina(0x800000,(uint )pagDir, mmu_xy2fisica(perro->jugador->x_cucha,perro->jugador->y_cucha),0x007); 	// Y ESTO RESULTA AHORA IRRELEVANTE

	mmu_mapear_pagina(0x401000, (uint) pagDir, mmu_xy2fisica(perro->jugador->x_cucha,perro->jugador->y_cucha), 0x007);
    uint viejo_cr3 = rcr3();
	mmu_mapear_pagina(0x401000, (uint) 0x27000, mmu_xy2fisica(perro->jugador->x_cucha,perro->jugador->y_cucha), 0x007);
    lcr3(0x27000);
	copiar_codigo((unsigned int *)aCopiar,(unsigned int *)0x401000);
    lcr3(viejo_cr3);

   


	tlbflush();
	return (uint ) pagDir;
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

void mmu_mover_perro(perro_t *perro, uint viejo_x, uint viejo_y){
	
	//mmu_mapear_pagina(mmu_xy2fisica(perro->x,perro->y), rcr3(), mmu_xy2fisica(perro->x,perro->y),0x007);
	mmu_mapear_pagina(0x401000, rcr3(), mmu_xy2fisica(perro->x,perro->y),0x007);
	copiar_codigo((uint *)mmu_xy2virtual(viejo_x,viejo_y),(uint *)0x401000); 

	mmu_mapear_pagina(mmu_xy2fisica(perro->x,perro->y), rcr3(), mmu_xy2virtual(perro->x,perro->y),0x007);
	//breakpoint();

}