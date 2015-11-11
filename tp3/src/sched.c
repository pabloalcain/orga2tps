/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "screen.h"

sched_t scheduler;

void sched_inicializar()
{
	scheduler.tasks[MAX_CANT_TAREAS_VIVAS].gdt_index = COD_TAREA_IDLE_DIR; // tarea idle
	scheduler.tasks[MAX_CANT_TAREAS_VIVAS].perro = NULL; // tarea idle

	int i;
	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
		scheduler.tasks[i].gdt_index = (GDT_IDX_TSS_BASE_PERROS_A + i);	// inicializar scheduler con la dir en la gdt de las tareas
		scheduler.tasks[i].perro = NULL;
		scheduler.tasks[MAX_CANT_PERROS_VIVOS + i].gdt_index = (GDT_IDX_TSS_BASE_PERROS_B + i);
		scheduler.tasks[MAX_CANT_PERROS_VIVOS + i].perro = NULL;
	}

	scheduler.current = 0; // se comienza a ejecutar la tarea idle
}


int sched_buscar_indice_tarea(uint gdt_index) {
    return MAX_CANT_TAREAS_VIVAS;
}


int sched_buscar_tarea_libre()
{
	int i = 1;

	while ((scheduler.tasks[i].perro != NULL) || (i < MAX_CANT_TAREAS_VIVAS)) {
		i++;
	}

    return (i % (MAX_CANT_TAREAS_VIVAS + 1)); // devuelve un indice de una tarea que esta libre o si recorre
    										  // todas las posiciones, devuelvo el indice de la tarea idle
}



perro_t* sched_tarea_actual()
{
    return scheduler.tasks[scheduler.current].perro;
}

void sched_agregar_tarea(perro_t *perro)
{
}

void sched_remover_tarea(unsigned int gdt_index)
{
}


uint sched_proxima_a_ejecutar()
{
	// busca el indice de la tarea a ejecutar segun scheduler.current y el turno del jugador
	perro_actual * = sched_tarea_actual;
	if (scheduler.indice_jugador_actual == JUGADOR_B) { // en este caso devuelvo el próximo perro a ejecutar del jugador A
		int i = scheduler.current + 1; // para revisar los indices siguientes, ya sé que el actual es del jugador actual
		while (i < MAX_CANT_PERROS_VIVOS) { // " + 1" porque el primer indice corresponde a la tarea idle
			if (scheduler.tasks[i].perro.index == i && i != 0) { // busco los primeros 8 (8 == MAX_CANT_PERROS_VIVOS) porque los primeros 8 perros son del jugador A
				return i;
			} 
		} 
		i = 1;
		while ()
	} else { // devuelvo el proximo del jugador B

	}
}


ushort sched_atender_tick()
{
    return scheduler.tasks[scheduler.current].gdt_index;
}


