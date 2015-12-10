/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
// #include "screen.h"

//sched_t scheduler;

void sched_inicializar()
{
	scheduler.tasks[MAX_CANT_TAREAS_VIVAS].gdt_index = COD_TAREA_IDLE_DIR; // tarea idle
	scheduler.tasks[MAX_CANT_TAREAS_VIVAS].perro = NULL; // tarea idle
	scheduler.jugador_actual = -1;
	int i;
	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
		scheduler.tasks[i].gdt_index = (GDT_IDX_TSS_BASE_PERROS_A + i);	// inicializar scheduler con la dir en la gdt de las tareas
		scheduler.tasks[i].perro = NULL;
		scheduler.tasks[MAX_CANT_PERROS_VIVOS + i].gdt_index = (GDT_IDX_TSS_BASE_PERROS_B + i);
		scheduler.tasks[MAX_CANT_PERROS_VIVOS + i].perro = NULL;
	}

	scheduler.current = 0; // se comienza a ejecutar la tarea idle
	scheduler.indice_ultimo_jugador_A = 1;
	scheduler.indice_ultimo_jugador_B = 9;
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
	int i = sched_buscar_tarea_libre2(perro->jugador->index);
	//breakpoint();
	if(i != 0){
	scheduler.tasks[i].perro = perro;
	}
}



int sched_buscar_tarea_libre2(uint jugador)
{
	int i;
	if(jugador == JUGADOR_A){
		i = 1;
	}else{
		i = 9;
	}
	int j = 0;
	while(j<8){
		if(scheduler.tasks[i].perro == NULL){
			return i;
		}
	i++;
	j++;
	}
	return 0;
}







void sched_remover_tarea(unsigned int jugador)
{
	if(jugador == JUGADOR_A){
		jugadorA.cant_perros_vivos--;
		unsigned char indice_aux = jugadorA.indice_perro_actual;
		jugadorA.perros[indice_aux].libre = TRUE;
	 } else{
		jugadorB.cant_perros_vivos--;
		jugadorB.perros[jugadorB.indice_perro_actual].libre = TRUE;
	}

}


uint sched_proxima_a_ejecutar()
{
	if (get_cant_perros_activos() == 0) {
		return GDT_IDX_TSS_TAREA_IDLE;
	}
	if (scheduler.jugador_actual == JUGADOR_B) {
		scheduler.jugador_actual = JUGADOR_A;
		//scheduler.ultimo_jugador = JUGADOR_A;
		int indice_perro = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_A);
		if (indice_perro == PERRO_NOT_FOUND) {
			//scheduler.ultimo_jugador = JUGADOR_B;
			jugadorB.indice_perro_actual = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_B); /* Caso cuando solo un jugador tiene perros activos */
			scheduler.current = jugadorB.indice_perro_actual;
			scheduler.tasks[scheduler.current].perro = &jugadorB.perros[scheduler.current];
			game_perro_actual = scheduler.tasks[scheduler.current].perro;
			return GDT_IDX_TSS_BASE_PERROS_B + jugadorB.indice_perro_actual;
		}
		jugadorA.indice_perro_actual = indice_perro;
		scheduler.current = indice_perro;
		scheduler.tasks[scheduler.current].perro = &jugadorA.perros[scheduler.current];
		game_perro_actual = scheduler.tasks[scheduler.current].perro;
		//scheduler.jugador_actual = JUGADOR_A;
		return GDT_IDX_TSS_BASE_PERROS_A + indice_perro;
	} else
	if (scheduler.jugador_actual == JUGADOR_A) {
		//scheduler.ultimo_jugador = JUGADOR_B;
		scheduler.jugador_actual = JUGADOR_B;
		int indice_perro = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_B);
		if (indice_perro == PERRO_NOT_FOUND) {
			//scheduler.ultimo_jugador = JUGADOR_A;
			jugadorA.indice_perro_actual = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_A);  /* Caso cuando solo un jugador tiene perros activos */
			scheduler.current = jugadorA.indice_perro_actual;
			scheduler.tasks[scheduler.current].perro = &jugadorA.perros[scheduler.current];
			game_perro_actual = scheduler.tasks[scheduler.current].perro;
			//scheduler.jugador_actual = JUGADOR_B;
			return GDT_IDX_TSS_BASE_PERROS_A + jugadorA.indice_perro_actual; 
		}
		jugadorB.indice_perro_actual = indice_perro;
		scheduler.current = indice_perro;
		scheduler.tasks[scheduler.current].perro = &jugadorB.perros[scheduler.current];
		game_perro_actual = scheduler.tasks[scheduler.current].perro;
		//scheduler.jugador_actual = JUGADOR_B;
		return GDT_IDX_TSS_BASE_PERROS_B + indice_perro;
	}
	return 	GDT_IDX_TSS_TAREA_IDLE;
}


ushort sched_atender_tick(ushort x)
{
	game_atender_tick(game_perro_actual);
    //return sched_proxima_a_ejecutar();
    return x;
}

unsigned int get_cant_perros_activos(){
	return jugadorB.cant_perros_vivos + jugadorA.cant_perros_vivos;
}

uint sched_jugador_actual() {
	return scheduler.jugador_actual;
}

uint sched_idle() {
	return GDT_IDX_TSS_TAREA_IDLE;
}
