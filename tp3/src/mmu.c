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
#define SIZE_W					80
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

void mmu_mapear_areas_de_kernel_y_libre(unsigned int cr3){
	int i = 0;
	for(; i < 0x400; i++){
		mmu_mapear_pagina(i*0x1000, (page_directory_entry *)cr3, i*0x1000, 1, 0);
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

void mmu_mapear_pagina(uint virtual, page_directory_entry * cr3, uint fisica, uchar rw, uchar user_supervisor)
{

    uint directorio = (virtual >> 22) & 0x3ff;
    uint tabla = (virtual >> 12) & 0x3ff;

    page_table_entry * page_table;

    if(!cr3[directorio].present)
    {
        page_table = (page_table_entry *) mmu_solicitar_pagina_nueva();

        cr3[directorio].present = 1;
        cr3[directorio].rw = rw;
        cr3[directorio].user_supervisor = user_supervisor;
        cr3[directorio].base_dir = ((uint) page_table) >> 12;
        cr3[directorio].ignored = 0;
    }
    else
    {
        page_table = (page_table_entry *) (cr3[directorio].base_dir << 12);
    }

    page_table[tabla].user_supervisor = user_supervisor;
    page_table[tabla].rw = rw;
    page_table[tabla].base_dir = fisica >> 12;
    page_table[tabla].present = 1;	

    tlbflush();
	
}


void mmu_unmapear_pagina(uint virtual, page_directory_entry * cr3)
{
    uint directorio = (virtual >> 22) & 0x3ff;
    uint tabla = (virtual >> 12) & 0x3ff;

    page_table_entry * page_table = (page_table_entry *) (cr3[directorio].base_dir << 12);

    page_table[tabla].present = 0;

    tlbflush();
}

void copiar_codigo(uint src, uint dst){

	uint *sr = (uint *) src;
	uint *ds = (uint *) dst;
	int i = 0;
	while(i<1024){
		ds[i] = sr[i];
		i++;
	}
}



uint mmu_xy2fisica(uint x, uint y){
	return 0x500000+x*4096+y*4096*80;
}

uint mmu_xy2virtual(uint x, uint y){
	return 0x800000+x*4096+y*4096*80;
}


void mmu_inicializar_tabla_kernel_para_perro(page_table_entry * tabla)
{
    uint i;
    for(i = 0; i<1024; i++)
    {
        tabla[i].base_dir = i;
        tabla[i].present = 1;
        tabla[i].rw = 0;  
        tabla[i].user_supervisor = 1; 
    }
}

/* Funcion para inicializar un perro en particular*/
page_directory_entry * mmu_inicializar_memoria_perro(jugador_t * jugador, perro_t * perro, uint xparam, uint yparam)
{
    //obtengo la siguiente libre
    page_directory_entry * resultado = (page_directory_entry *) mmu_solicitar_pagina_nueva();
    // dar_siguiente lo devuelve en 0

    // me armo la tabla del kernel
    page_table_entry * tabla_kernel = (page_table_entry *) mmu_solicitar_pagina_nueva();
    mmu_inicializar_tabla_kernel_para_perro(tabla_kernel);


    resultado[0].base_dir = ((uint) tabla_kernel)>>12;
    resultado[0].rw = 0;
    resultado[0].present = 1;
    resultado[0].user_supervisor = 0; 
    resultado[0].ignored = 0;


    uint codigo_tarea;
    if(jugador->index == JUGADOR_A)
    {
        if(perro->tipo == TIPO_1) codigo_tarea =  ADDR_COD_TAREA_A1;
        else codigo_tarea = ADDR_COD_TAREA_A2;
    }
    else
    {
        if(perro->tipo == TIPO_2) codigo_tarea = ADDR_COD_TAREA_B1;
        else codigo_tarea = ADDR_COD_TAREA_B2;
    }

    uint x;
    uint y;
    x = jugador->x;
    y = jugador->y;
    //mapeo paginas
    mmu_mapear_pagina(0x401000, resultado, game_xy2lineal(x,y)*0x1000+0x500000, 1, 1);



    //copio el codigo
    mmu_mapear_pagina(0x401000, (page_directory_entry *) 0x27000, game_xy2lineal(x,y)*0x1000+0x500000, 1, 0);
    copiar_codigo(codigo_tarea, 0x401000);
    *((uint *) 0x401ffc) = yparam;  // parametros que toma
    *((uint *) 0x401ff8) = xparam;  // la tarea
    *((uint *) 0x401ff4) = 0;       //direccion de retorno, es fruta
    mmu_unmapear_pagina(0x401000, (page_directory_entry *) 0x27000);


    return resultado;
}

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
    
    mmu_mapear_pagina(mmu_xy2virtual(perro->x,perro->y), ( page_directory_entry *) rcr3(), mmu_xy2fisica(viejo_x,viejo_y), 1, 1);
    mmu_mapear_pagina(0x401000, ( page_directory_entry *) rcr3(), mmu_xy2fisica(perro->x,perro->y), 1, 1);
    copiar_codigo(mmu_xy2virtual(perro->x,perro->y), 0x401000); 
	//mmu_mapear_pagina(mmu_xy2fisica(perro->x,perro->y), ( page_directory_entry *) rcr3(), mmu_xy2virtual(perro->x,perro->y), 0, 1);
	//mmu_mapear_pagina(mmu_xy2virtual(perro->x,perro->y), ( page_directory_entry *) rcr3(), mmu_xy2fisica(perro->x,perro->y), 0, 1 );
	//breakpoint();
}