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

}

void sched_remover_tarea(unsigned int jugador, tss* debug_tss_dir)
{
	   unsigned int debug_jugador_actual;
	   unsigned int debug_tipo_perro;
	   tss debug_tss = *debug_tss_dir;;
	   screen_guardar_estado_actual_pantalla();
	if(jugador == JUGADOR_A){
		debug_tipo_perro = jugadorA.perros[jugadorA.indice_perro_actual].tipo;
		jugadorA.cant_perros_vivos--;
		unsigned char indice_aux = jugadorA.indice_perro_actual;
		jugadorA.perros[indice_aux].libre = TRUE;
	 	debug_jugador_actual = JUGADOR_A;
	 } else{
	 	debug_tipo_perro = jugadorB.perros[jugadorB.indice_perro_actual].tipo;
		jugadorB.cant_perros_vivos--;
		jugadorB.perros[jugadorB.indice_perro_actual].libre = TRUE;
	 	debug_jugador_actual = JUGADOR_B;
	}
	if(modo_debug == TRUE){
		screen_mostrar_cartel(debug_jugador_actual, debug_tipo_perro, debug_tss);
		pausa = TRUE;
	}
}


uint sched_proxima_a_ejecutar()
{
	if (scheduler.jugador_actual == JUGADOR_B) {
		scheduler.ultimo_jugador = JUGADOR_A;
		int indice_perro = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_A);
		if (indice_perro == PERRO_NOT_FOUND) {
			scheduler.ultimo_jugador = JUGADOR_B;
			jugadorB.indice_perro_actual = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_B); /* Caso cuando solo un jugador tiene perros activos */
			return GDT_IDX_TSS_BASE_PERROS_B + jugadorB.indice_perro_actual;
		}
		jugadorA.indice_perro_actual = indice_perro;
		return 1 + indice_perro;
	} else {
		scheduler.ultimo_jugador = JUGADOR_B;
		int indice_perro = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_B);
		if (indice_perro == PERRO_NOT_FOUND) {
			scheduler.ultimo_jugador = JUGADOR_A;
			jugadorA.indice_perro_actual = jugador_obtener_proximo_perro_a_ejecutar(JUGADOR_A); /* Caso cuando solo un jugador tiene perros activos */
			return GDT_IDX_TSS_BASE_PERROS_A + jugadorA.indice_perro_actual; 
		}
		jugadorB.indice_perro_actual = indice_perro;
		return GDT_IDX_TSS_BASE_PERROS_B + indice_perro;
	}
}


ushort sched_atender_tick()
{
	perro_t* perro_actual = sched_tarea_actual();
	game_atender_tick(perro_actual);
    return sched_proxima_a_ejecutar();
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