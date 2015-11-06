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
	scheduler.tasks[0].gdt_index = COD_TAREA_IDLE_DIR; // tarea idle
	scheduler.tasks[0].perro = NULL; // tarea idle

	scheduler.current = 0; // se comienza a ejecutar la tarea idle
}


int sched_buscar_indice_tarea(uint gdt_index) {
    return MAX_CANT_TAREAS_VIVAS;
}


int sched_buscar_tarea_libre()
{
	int i = 1;

	while ((scheduler.tasks[i].perro != NULL) || (i < MAX_CANT_TAREAS_VIVAS+1)) {
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
    return MAX_CANT_TAREAS_VIVAS;
}


ushort sched_atender_tick()
{
    return scheduler.tasks[scheduler.current].gdt_index;
}


