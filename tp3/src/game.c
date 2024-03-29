/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
// #include "mmu.h"
// #include "tss.h"
#include "screen.h"

#include <stdarg.h>

unsigned int modo_debug ;
unsigned int pausa ;

int escondites[ESCONDITES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, HUESOS)
                                        {76,  25, 1}, {12, 15, 1}, {9, 10, 1}, {47, 21, 1} ,
                                        {34,  11, 1}, {75, 38, 1}, {40, 21, 1}, {72, 17, 1}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;

typedef struct posicion_x_y
{

    uint x;
    uint y;

  	uint id_orden;

} posicion_x_y;

perro_t *game_perro_actual = NULL;
int ultimo_cambio = MAX_SIN_CAMBIOS;

// void ASSERT_OR_ERROR(uint value, char* error_msg)
// {
// 	if (!value) {
// 		print(error_msg, 5, 5, C_BG_LIGHT_GREY | C_FG_BLACK);
// 		breakpoint();
// 	}
// }

void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_es_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}


void game_inicializar()
{
  int i;
  huesos_totales = 0;
	modo_debug = FALSE;
	pausa = FALSE;
	game_jugador_inicializar(&jugadorA, JUGADOR_A );
	game_jugador_inicializar(&jugadorB, JUGADOR_B );
  screen_pintar_puntajes();
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
	{
    huesos_totales += escondites[i][2];
	}
}


// devuelve la cantidad de huesos que hay en la posición pasada como parametro
uint game_huesos_en_posicion(uint x, uint y) {
	int i;
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
	{
		if (escondites[i][0] == x && escondites[i][1] == y)
			return escondites[i][2];
	}
	return 0;
}

void game_restar_hueso_en_posicion(uint x, uint y) {
	int i;
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
    {
      if (escondites[i][0] == x && escondites[i][1] == y){
        escondites[i][2] = escondites[i][2] - 1;
      }
    }
}


// devuelve algun perro que esté en la posicion pasada (hay max 2, uno por jugador)
perro_t* game_perro_en_posicion(uint x, uint y)
{
//	breakpoint();
	int i;
	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++)
	{
		if (!jugadorA.perros[i].libre && jugadorA.perros[i].x == x && jugadorA.perros[i].y == y)
			return &jugadorA.perros[i];
		if (!jugadorB.perros[i].libre && jugadorB.perros[i].x == x && jugadorB.perros[i].y == y)
			return &jugadorB.perros[i];
	}
	return NULL;
}

unsigned int game_en_pausa() {
	return pausa;
}

// termina si se agotaron los huesos o si hace tiempo que no hay ningun cambio
void game_terminar_si_es_hora()
{
}

int jugador_obtener_proximo_perro_a_ejecutar(unsigned int jugador_actual) {
	if (jugador_actual == JUGADOR_A) {
		int i;
		for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
			int posicion = (jugadorA.indice_perro_actual + i + 1) % MAX_CANT_PERROS_VIVOS;
			if (jugadorA.perros[posicion].libre != TRUE) {
				return posicion;
			}
		}
	} else {
		int i;
		for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
			int posicion = (jugadorB.indice_perro_actual + i + 1) % MAX_CANT_PERROS_VIVOS;
			if (jugadorB.perros[posicion].libre != TRUE) {
				return posicion;
			}
		}
	}
	return PERRO_NOT_FOUND;
}

uint game_perro_recibir_orden(perro_t *perro) {
	jugador_t* jugador = perro->jugador;
	ushort orden = jugador->ultima_orden;
	posicion_x_y resultado;
	resultado.x = perro->x;
	resultado.y = perro->y;
	resultado.id_orden = orden;
	return (resultado.id_orden << 16 | resultado.y << 8 | resultado.x);
}

unsigned int game_get_is_modo_debug() {
	return modo_debug;
}

void game_switch_modo_debug() {
	if(modo_debug == 1) {
		modo_debug = 0; 
		pausa = 0;
		screen_reestablecer_pantalla();
	} else {
		modo_debug = 1;
		pausa = 0;
	}
}

void set_pausa() {
	pausa = TRUE;
}

jugador_t game_get_jugador_from_perro(perro_t *perro) {
	return *(perro->jugador);
}

uint game_get_tipo_perro(perro_t *perro) {
	return perro->tipo;
}
