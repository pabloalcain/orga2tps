
#include "game.h"
// #include "mmu.h"
#include "screen.h"
#include "sched.h"

#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

void game_jugador_inicializar(jugador_t *j, uint tipo_jugador)
{

	j->index = tipo_jugador;
	j->cant_perros_vivos = 0;
	if (j->index == JUGADOR_A) {
		j->x = POS_INIT_A_X;
		j->y = POS_INIT_A_Y;

	} else {
		j->x = POS_INIT_B_X;
		j->y = POS_INIT_B_Y;
	}

	j->puntos = 0;
	j->x_cucha = j->x;
	j->y_cucha = j->y;

	uint gdt_index;

	int i;

	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++)
	{
		if (j->index == JUGADOR_A) {
			gdt_index = (GDT_IDX_TSS_BASE_PERROS_A + i);
		} else {
			gdt_index = (GDT_IDX_TSS_BASE_PERROS_B + i);
		}
		game_perro_inicializar(&j->perros[i], j, i, gdt_index + i*8);
	}

}


// debe devolver el proximo perro del arreglo que no esté siendo usado actualmente
perro_t* game_jugador_dame_perro_libre(jugador_t *j)
{
	int i = 0;

	for (; i < MAX_CANT_PERROS_VIVOS; i++)
	{
		if (j->perros[i].libre == TRUE)
			return &j->perros[i];
	}

	return NULL;
}


// debe encargarse de buscar un perro libre, configurarlo, y inicializar su mapeo de memoria, tss, y lugar en el sched
void game_jugador_lanzar_perro(jugador_t *j, uint tipo, int x, int y)
{
	if (game_perro_en_posicion(x,y) != NULL)
		return;

	perro_t *perro = game_jugador_dame_perro_libre(j);
	if (perro == NULL)
		return;
	
	game_perro_reciclar_y_lanzar(perro, tipo);
	j->cant_perros_vivos++;
	if (scheduler.jugador_actual == -1) {
		if (j->index == JUGADOR_A)
		{
			scheduler.jugador_actual = JUGADOR_B; // para que el scheduler busque el a
		} else 
		{
			scheduler.jugador_actual = JUGADOR_A;
		}
		
	}
}

// recibe un par (x, y) y un jugador, al cual debe mover en esa dirección
uint game_jugador_moverse(jugador_t *j, int x, int y)
{
	int nuevo_x = j->x + x;
	int nuevo_y = j->y + y;
	if((nuevo_x<0 || nuevo_y<0) ||(nuevo_x >= MAPA_ANCHO || nuevo_y >= MAPA_ALTO)) return 0;

	screen_borrar_jugador(j);

	j->x = nuevo_x;
	j->y = nuevo_y;
	
	screen_pintar_jugador(j);
    // ~~~ completar ~~~
    return nuevo_x + nuevo_y; // uso todas las variables locales para que no tire warning -> error
}

// descarga 1 hueso en la cucha y actualiza el screen
void game_jugador_anotar_punto(jugador_t *j)
{
  ultimo_cambio = MAX_SIN_CAMBIOS;
  
	j->puntos++;
  
  screen_pintar_puntajes();

	if(j->puntos==999)
		screen_stop_game_show_winner(j);
}


// guarda la orden en el jugador para que los perros puedan preguntarla luego (mediante un syscall)
void game_jugador_dar_orden(jugador_t *jugador, int orden)
{
	jugador->ultima_orden = orden;	
}


