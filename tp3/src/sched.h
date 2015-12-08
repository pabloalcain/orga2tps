/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"
// #include "gdt.h"
//#include "tss.h"



#define MAX_CANT_TAREAS_VIVAS   (MAX_CANT_PERROS_VIVOS * 2)

/*
================================================================================
                       ~~~ estructuras del scheduler ~~~
================================================================================
*/

// para el scheduler, lo importante de cada tarea es su indice en la gdt y
// el perro al cual está asociada. Si el indice es null significará que está libre.
typedef struct sched_task_t
{
    unsigned int gdt_index;
	perro_t *perro;
} sched_task_t;

// el scheduler posee un arreglo de tareas (cada una puede estar libre o asignada)
typedef struct sched_t
{
    sched_task_t tasks[MAX_CANT_TAREAS_VIVAS+1];
    ushort current;
    ushort jugador_actual; // indica el jugador actual (JUGADOR_A o JUGADOR_B)
    ushort ultimo_jugador; // indica el ultimo jugador jugador (JUGADOR_A o JUGADOR_B)
    uint indice_jugador_actual;
    uint indice_ultimo_jugador_A; // valores válidos: de 1 a 8 inclusive
    uint indice_ultimo_jugador_B; // valores válidos: de 9 a 16 inclusive
} sched_t;

extern sched_t scheduler;

/*
================================================================================
                       ~~~ funciones del scheduler ~~~
================================================================================
*/

// debe agregar una tarea al scheduler, buscando un slot libre y asociando indice de gdt y perro 
void sched_agregar_tarea(perro_t *perro);

// debe remover tareas del scheduler
void sched_remover_tarea(unsigned int jugador);

// debe devolver el perro correspondiente a la tarea que está corriendo actualmente
perro_t* sched_tarea_actual();

// debe avisar al juego que hubo un tick (para que haga cosas del juego) y luego configurarse
// para pasar a la siguiente tarea (devuelve su indice en la gdt)
ushort sched_atender_tick();

uint sched_jugador_actual();

int sched_buscar_tarea_libre2(uint jugador);

#endif	/* !__SCHED_H__ */
