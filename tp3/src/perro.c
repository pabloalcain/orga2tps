// #include "game.h"
#include "mmu.h"
 #include "screen.h"
#include "tss.h"
#include "sched.h"

#define PERROS_LIMIT_REACHED   -1;


typedef struct posicion_x_y
{
    unsigned int x;
	unsigned int y;
	unsigned int id_orden;
} posicion_x_y;

// realiza inicialización básica de un perro. El perro aun no está vivo ni por lanzarse. Setea jugador, indice, etc
void game_perro_inicializar(perro_t *perro, jugador_t *j, uint index, uint id)
{
	perro->id   = id;
    perro->index = index;
    perro->jugador = j;
	perro->libre = TRUE;

    perro->x = j->x_cucha;
    perro->y = j->y_cucha;

    perro->huesos = 0; // perro.huesos vale a lo sumo 10

  // ~~~ para ser completado ~~~

}


int jugador_get_indice_perro_nuevo(perro_t *perro){
	int i;
	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
		if (perro[i].libre == TRUE) return i;
	}
	return PERROS_LIMIT_REACHED;
}


// toma un perro ya existente libre y lo recicla seteando x e y a la cucha.
// luego inicializa su mapeo de memoria, tss, lo agrega al sched y pinta la pantalla
void game_perro_reciclar_y_lanzar(perro_t *perro, uint tipo)
{
	jugador_t *j = perro->jugador;

	perro->x = j->x_cucha;
	perro->y = j->y_cucha;
	perro->tipo = tipo;
	perro->libre = FALSE;

	// ahora debo llamar a rutinas que inicialicen un nuevo mapa de
	// memoria para el nuevo perro, que carguen su tss correspondiente,
	// lo scheduleen y finalmente lo pinten en pantalla

	// ~~~ completar ~~~
	if ((perro->jugador->index) == JUGADOR_A) {
		page_directory_entry * cr3 = mmu_inicializar_memoria_perro(j, perro, j->x_cucha, j->y_cucha);
		tss_inicializar_tarea_perro(perro->index, A, cr3);
	} else {
		page_directory_entry * cr3 = mmu_inicializar_memoria_perro(j, perro, j->x_cucha, j->y_cucha);
		tss_inicializar_tarea_perro(perro->index, B, cr3);
	}

	sched_agregar_tarea(perro);
	screen_pintar_perro(perro);
	screen_actualizar_reloj_perro(perro);

}

// el perro descargó sus huesos o realizó una acción no válida y caputó, hay que sacarlo del sistema.
void game_perro_termino(perro_t *perro)
{
//	~~~ completar ~~~
	scheduler.tasks[perro->index].perro = NULL; 
	perro->libre = TRUE;
	screen_pintar_reloj_perro(perro);
	jugador_t* j = perro->jugador;
	j->cant_perros_vivos--;
}

// transforma código de dirección en valores x e y 
uint game_dir2xy(/* in */ direccion dir, /* out */ int *x, /* out */ int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

// recibe una direccion y un perro, al cual debe mover en esa dirección
// *** viene del syscall mover ***
uint game_perro_mover(perro_t *perro, direccion dir)
{

	int x, y;
	uint res = game_dir2xy(dir, &x, &y);
	int nuevo_x = perro->x + x;
	int nuevo_y = perro->y + y;
    int viejo_x = perro->x;
    int viejo_y = perro->y;
    // ~~~ completar ~~~
   	if (game_perro_en_posicion(nuevo_x,nuevo_y) != NULL){ 			
   // 		if (!(game_perro_en_posicion(nuevo_x,nuevo_y)->jugador == perro->jugador))
   // 		{ 	
   // 			perro->x = nuevo_x;
   // 			perro->y = nuevo_y;
   // 			mmu_mover_perro(perro, viejo_x, viejo_y);
			// screen_pintar_perro(perro);
			// //screen_actualizar_posicion_mapa(perro->x, perro->y);
   // 			if((nuevo_x<1 || nuevo_y<1) || (nuevo_x>79 || nuevo_y>49) || (nuevo_x == perro->jugador->x_cucha && nuevo_y == perro->jugador->y_cucha)){
   // 				game_perro_termino(perro);
   // 			}
   // 		}
   		game_perro_termino(perro);
   	} else {
   		screen_borrar_perro(perro);
   		perro->x = nuevo_x;
		perro->y = nuevo_y;
		mmu_mover_perro(perro, viejo_x, viejo_y);		
		
		screen_pintar_perro(perro);
		//screen_actualizar_posicion_mapa(perro->x, perro->y);
		
		if((nuevo_x<1 || nuevo_y<1) ||(nuevo_x>79 || nuevo_y>49) || (nuevo_x == perro->jugador->x_cucha && nuevo_y == perro->jugador->y_cucha)){
			game_perro_termino(perro);
		}
   	}
    

    return nuevo_x + nuevo_y + viejo_x + viejo_y + res; // uso todas las variables para que no tire warning->error.
}
// recibe un perro, el cual debe cavar en su posición
// *** viene del syscall cavar ***
uint game_perro_cavar(perro_t *perro)
{
	// ~~~ completar ~~~
	uint huesos_aqui = game_huesos_en_posicion(perro->x, perro->y);
	if (huesos_aqui > 0 && perro->huesos <= 10) {
		perro->huesos ++;
		game_restar_hueso_en_posicion(perro->x, perro->y);
	}
	return 0;
}

// recibe un perro, devueve la dirección del hueso más cercano
// *** viene del syscall olfatear ***
uint game_perro_olfatear(perro_t *perro)
{
	int x_actual_diff = 1000, y_actual_diff = 1000;


	int i;
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
	{
		if (escondites[i][2] == 0) continue;

		int diff_x = escondites[i][0] - (int)perro->x;
		int diff_y = escondites[i][1] - (int)perro->y;

		if (x_actual_diff * x_actual_diff + y_actual_diff * y_actual_diff > diff_x * diff_x + diff_y * diff_y)
		{
			x_actual_diff = diff_x;
			y_actual_diff = diff_y;
		}
   	}

	if (x_actual_diff == 0 && y_actual_diff == 0)
		return AQUI;

	if (x_actual_diff * x_actual_diff > y_actual_diff * y_actual_diff)
	{
		return x_actual_diff > 0 ? DER : IZQ;
	}
	else 
	{
		return y_actual_diff > 0 ? ABA : ARR;
	}

    return 0;
}


// chequea si el perro está en la cucha y suma punto al jugador o lo manda a dormir
void game_perro_ver_si_en_cucha(perro_t *perro)
{
	if (perro->x != perro->jugador->x_cucha || perro->y != perro->jugador->y_cucha)
		return;

	if (perro->huesos == 0)
		return;

	game_jugador_anotar_punto(perro->jugador);
	perro->huesos--;
	if (perro->huesos == 0)
		game_perro_termino(perro);
}

