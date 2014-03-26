#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define N 20
#define EXIT 27
#define DERECHA 100 //la d
#define IZQUIERDA 97 //la a
#define ARRIBA 119 //la w
#define ABAJO 115 //la s
#define DISPARAR1 48 //numero 0
#define DISPARAR2 32 //barra espaciadora
#define MAXAMO 2

enum Direccion{norte = 1, sur, este, oeste};

struct arma {
    Direccion direccion;
    int col, fila;
    bool se_mueve;
};

void inicializar_bala(arma bala[], int pos_jugadores[], int bala_disparada){
    switch(bala[bala_disparada].direccion){
	case norte:
	    bala[bala_disparada].col = pos_jugadores[1];
	    bala[bala_disparada].fila = pos_jugadores[0]-1;
	    break;
	case sur:
	    bala[bala_disparada].col = pos_jugadores[1];
	    bala[bala_disparada].fila = pos_jugadores[0]+1;
	    break;
	case este:
	    bala[bala_disparada].col = pos_jugadores[1]+1;
	    bala[bala_disparada].fila = pos_jugadores[0];
	    break;
	case oeste:
	    bala[bala_disparada].col = pos_jugadores[1]-1;
	    bala[bala_disparada].fila = pos_jugadores[0];
	    break;
    }

}
int movimiento(){
    int respuesta;
    //coge los caracteres introducidos por el usuario
    respuesta = getch();//coge los caracteres
    //compara que la respuesta es correcta
    return respuesta;
}
void mover_jugadores(int respuesta, char tablero[][N], int pos_jugadores[][2], Direccion apunta[], 
	arma bala[][MAXAMO], int balas_disparadas[]){

    static Direccion direccion_anterior[2];

    //borra el arma
    for(int borrar=0; borrar<2; borrar++)
	switch(direccion_anterior[borrar]){
	    case este:
		tablero[pos_jugadores[borrar][0]][pos_jugadores[borrar][1]+1] = ' ';
		break;
	    case oeste:
		tablero[pos_jugadores[borrar][0]][pos_jugadores[borrar][1]-1] = ' ';
		break;
	    case norte:
		tablero[pos_jugadores[borrar][0]-1][pos_jugadores[borrar][1]] = ' ';
		break;
	    case sur:
		tablero[pos_jugadores[borrar][0]+1][pos_jugadores[borrar][1]] = ' ';
		break;
	}

    switch(respuesta){

	case KEY_RIGHT:
	    if(tablero[pos_jugadores[0][0]][pos_jugadores[0][1]+2] != 'H'){
		tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = ' ';
		pos_jugadores[0][1] ++;
		apunta[0] = este;
	    }
	    break;

	case DERECHA:
	    if(tablero[pos_jugadores[1][0]][pos_jugadores[1][1]+2] != 'H'){
		tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = ' ';
		pos_jugadores[1][1] ++;
		apunta[1] = este;
	    }
	    break;

	case KEY_LEFT:
	    if(tablero[pos_jugadores[0][0]][pos_jugadores[0][1]-2] != 'H'){
		tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = ' ';
		pos_jugadores[0][1] --;
		apunta[0] = oeste;
	    }
	    break;

	case IZQUIERDA:
	    if(tablero[pos_jugadores[1][0]][pos_jugadores[1][1] - 2] != 'H'){
		tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = ' ';
		pos_jugadores[1][1] --;
		apunta[1] = oeste;
	    }
	    break;

	case KEY_UP:
	    if(tablero[pos_jugadores[0][0] - 2][pos_jugadores[0][1]] != 'H'){

		tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = ' '; 
		pos_jugadores[0][0] --;
		apunta[0] = norte;
	    }
	    break;

	case ARRIBA:
	    if(tablero[pos_jugadores[1][0] - 2][pos_jugadores[1][1]] != 'H'){
		tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = ' ';
		pos_jugadores[1][0] --;
		apunta[1] = norte;
	    }
	    break;

	case KEY_DOWN:
	    if(tablero[pos_jugadores[0][0] + 2][pos_jugadores[0][1]] != 'H'){
		tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = ' ';
		pos_jugadores[0][0] ++;
		apunta[0] = sur;
	    }
	    break;

	case ABAJO:
	    if(tablero[pos_jugadores[1][0] + 2][pos_jugadores[1][1]] != 'H'){
		tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = ' ';
		pos_jugadores[1][0] ++;
		apunta[1] = sur;
	    }
	    break;
	case DISPARAR1:

	    if(!bala[0][balas_disparadas[0]].se_mueve){
		bala[0][balas_disparadas[0]].se_mueve = true;
		bala[0][balas_disparadas[0]].direccion = apunta[0];
		inicializar_bala(bala[0], pos_jugadores[0], balas_disparadas[0]);
		if(++balas_disparadas[0] == MAXAMO)
		    balas_disparadas[0] = 0;
	    }

	    break;

	case DISPARAR2:
	    if(!bala[1][balas_disparadas[1]].se_mueve){
		bala[1][balas_disparadas[1]].se_mueve = true;
		bala[1][balas_disparadas[1]].direccion = apunta[1];
		inicializar_bala(bala[1], pos_jugadores[1], balas_disparadas[1]);
		if(++balas_disparadas[1] == MAXAMO)
		    balas_disparadas[1] = 0;
	    }
	    break;

    }
    tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = '5';
    tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = '0';

    for(int pintar_arma=0; pintar_arma<2; pintar_arma++)
	switch(apunta[pintar_arma]){
	    case este:
		tablero[pos_jugadores[pintar_arma][0]][pos_jugadores[pintar_arma][1]+1] = '=';
		break;
	    case oeste:
		tablero[pos_jugadores[pintar_arma][0]][pos_jugadores[pintar_arma][1]-1] = '=';
		break;
	    case norte:
		tablero[pos_jugadores[pintar_arma][0]-1][pos_jugadores[pintar_arma][1]] = '=';
		break;
	    case sur:
		tablero[pos_jugadores[pintar_arma][0]+1][pos_jugadores[pintar_arma][1]] = '=';
		break;
	}

    direccion_anterior[0] = apunta[0];
    direccion_anterior[1] = apunta[1];


}
void mover_bala(arma bala[][MAXAMO], char tablero[][N], bool jugador_caido[], int puntuacion[]){

    for(int jugador=0; jugador<2; jugador++)
	for(int asignar=0; asignar<MAXAMO; asignar++)
	    if (bala[jugador][asignar].se_mueve){
		tablero[bala[jugador][asignar].fila][bala[jugador][asignar].col] = ' ';
		switch(bala[jugador][asignar].direccion){
		    case este:
			bala[jugador][asignar].col++;
			break;
		    case oeste:
			bala[jugador][asignar].col--;
			break;
		    case norte:
			bala[jugador][asignar].fila--;
			break;
		    case sur:
			bala[jugador][asignar].fila++;
			break;
		}

		if(tablero[bala[jugador][asignar].fila][bala[jugador][asignar].col] == 'H')
		    bala[jugador][asignar].se_mueve = false;
		else
		    if(tablero[bala[jugador][asignar].fila][bala[jugador][asignar].col] == '0' ||
			tablero[bala[jugador][asignar].fila][bala[jugador][asignar].col] == '5'){
			bala[jugador][asignar].se_mueve = false;
			if(jugador == 0){
			    jugador_caido[0] = true;
			    puntuacion[0]++;
			}
			else{
			    jugador_caido[1] = true;
			    puntuacion[1]++;
			}
		    }
		    else
			tablero[bala[jugador][asignar].fila][bala[jugador][asignar].col] = '*';
	    }

}
void pintar_tablero(char tablero[][N], int respuesta, int puntuacion[2]){
    
    clear();
    for(int x=0; x<N; x++){
	for(int y=0; y<N; y++)
	    printw("%c", tablero[x][y]);
	printw("\n");
    }
    printw("J1-%i  J2-%i\n", puntuacion[0], puntuacion[1]);
    refresh();
}
void bucle_juego(){

    int respuesta,
	balas_disparadas[2],
	puntuacion[2];
    int pos_jugadores[2][2];
    char tablero[N][N];
    Direccion apunta[2];
    arma bala[2][MAXAMO];
    bool jugador_caido[2];

    puntuacion[0] = puntuacion[1] = 0;
    do{
	balas_disparadas[0] = balas_disparadas[1] = 0;

	for(int m_jugador=0; m_jugador<2; m_jugador++)
	    for(int municion=0; municion<MAXAMO; municion++)
		bala[m_jugador][municion].se_mueve = false;

	pos_jugadores[0][0] = pos_jugadores[0][1] = 5;
	pos_jugadores[1][0] = pos_jugadores[1][1] = 10;

	for(int x=0; x<N; x++)
	    for(int y=0; y<N; y++)
		if(x == 0 || y == 0 || x == N-1 || y == N-1)
		    tablero[x][y] = 'H';
		else
		    tablero[x][y] = ' ';
	tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = '0';
	tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = '5';
	tablero[pos_jugadores[0][0]][pos_jugadores[0][1]+1] = 
	    tablero[pos_jugadores[1][0]][pos_jugadores[1][1]+1] = '=';
	apunta[0] = apunta[1] = este;


	jugador_caido[0] = jugador_caido[1] = false;

	do{
	    timeout( 100 );
	    pintar_tablero(tablero, respuesta, puntuacion);
	    respuesta = movimiento();
	    mover_jugadores(respuesta, tablero, pos_jugadores, apunta, bala, balas_disparadas);
	    mover_bala(bala, tablero, jugador_caido, puntuacion);
	}while(respuesta != EXIT && jugador_caido[0] != true && jugador_caido[1] != true);
    }while(respuesta != EXIT);
}
int main(int argc, char *argv[]){
    initscr();
    raw();
    keypad(stdscr, TRUE); 
    noecho();
    start_color();

    bucle_juego();

    endwin(); 
    return EXIT_SUCCESS;
}
