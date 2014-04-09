#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define N 20 //cantidad de celdas del mapa
#define EXIT 27
#define DERECHA 100 //la d
#define IZQUIERDA 97 //la a
#define ARRIBA 119 //la w
#define ABAJO 115 //la s
#define DISPARAR1 48 //numero 0
#define DISPARAR2 32 //barra espaciadora
#define MAXAMO 2 //municion maxima que se puede disparar
#define J1 0 //se refiere al jugador 1
#define J2 1
#define NJUGADORES 2
#define COORDENADAS 2
#define FILA 0
#define COLUMNA 1
#define CARACTERJ1 '0'
#define CARACTERJ2 '5'
#define CMUNICION '*'
#define ARMA '='

enum Direccion{norte = 1, sur, este, oeste};
enum Colores{VV = 1, AV, MV, NV, BV, RR};

struct arma {
    Direccion direccion;
    int col, fila;
    bool se_mueve;
};

void comprobacion(){

    int respuesta;

    if( LINES < N || COLS < N ){
	while( (LINES < N || COLS < N) && respuesta != EXIT){
	    erase();
	    mvprintw( LINES/2, COLS/2 - 15, "LINES %d >= %d -- COLS %d >= %d",
		    LINES, N, COLS, N );
	    respuesta = getch();
	    refresh();
	}
	clear();
    }
    if (respuesta == EXIT){
	endwin(); 
	exit(EXIT_FAILURE);
    }
}
//inicializa la direccion de la bala
void inicializar_bala(arma bala[], int pos_jugadores[], int bala_disparada){
    switch(bala[bala_disparada].direccion){
	case norte:
	    bala[bala_disparada].col = pos_jugadores[COLUMNA];
	    bala[bala_disparada].fila = pos_jugadores[FILA]-1;
	    break;
	case sur:
	    bala[bala_disparada].col = pos_jugadores[COLUMNA];
	    bala[bala_disparada].fila = pos_jugadores[FILA]+1;
	    break;
	case este:
	    bala[bala_disparada].col = pos_jugadores[COLUMNA]+1;
	    bala[bala_disparada].fila = pos_jugadores[FILA];
	    break;
	case oeste:
	    bala[bala_disparada].col = pos_jugadores[COLUMNA]-1;
	    bala[bala_disparada].fila = pos_jugadores[FILA];
	    break;
    }

}
void mover_jugadores(int respuesta, char tablero[][N], int pos_jugadores[][COORDENADAS], Direccion apunta[], 
	arma bala[][MAXAMO], int balas_disparadas[]){

    static Direccion direccion_anterior[2];

    //borra el arma
    for(int borrar=0; borrar<2; borrar++)
	switch(direccion_anterior[borrar]){
	    case este:
		tablero[pos_jugadores[borrar][FILA]][pos_jugadores[borrar][COLUMNA]+1] = ' ';
		break;
	    case oeste:
		tablero[pos_jugadores[borrar][FILA]][pos_jugadores[borrar][COLUMNA]-1] = ' ';
		break;
	    case norte:
		tablero[pos_jugadores[borrar][FILA]-1][pos_jugadores[borrar][COLUMNA]] = ' ';
		break;
	    case sur:
		tablero[pos_jugadores[borrar][FILA]+1][pos_jugadores[borrar][COLUMNA]] = ' ';
		break;
	}

    switch(respuesta){

	case KEY_RIGHT:
	    if(tablero[pos_jugadores[J1][FILA]][pos_jugadores[J1][1]+2] != 'H' &&
		    tablero[pos_jugadores[J1][FILA]][pos_jugadores[J1][1]+2] != CARACTERJ2){
		tablero[pos_jugadores[J1][FILA]][pos_jugadores[J1][1]] = ' ';
		pos_jugadores[J1][1] ++;
		apunta[J1] = este;
	    }
	    break;

	case DERECHA:
	    if(tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1]+2] != 'H' &&
		    tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1]+2] != CARACTERJ1){
		tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1]] = ' ';
		pos_jugadores[J2][1] ++;
		apunta[J2] = este;
	    }
	    break;

	case KEY_LEFT:
	    if(tablero[pos_jugadores[J1][0]][pos_jugadores[J1][1]-2] != 'H' &&
		    tablero[pos_jugadores[J1][0]][pos_jugadores[J1][1]-2] != CARACTERJ2){
		tablero[pos_jugadores[J1][0]][pos_jugadores[J1][1]] = ' ';
		pos_jugadores[J1][1] --;
		apunta[J1] = oeste;
	    }
	    break;

	case IZQUIERDA:
	    if(tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1] - 2] != 'H' &&
		    tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1]-2] != CARACTERJ1){
		tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1]] = ' ';
		pos_jugadores[J2][1] --;
		apunta[J2] = oeste;
	    }
	    break;

	case KEY_UP:
	    if(tablero[pos_jugadores[J1][0] - 2][pos_jugadores[J1][1]] != 'H' &&
		    tablero[pos_jugadores[J1][0]-2][pos_jugadores[J1][1]] != CARACTERJ2){
		tablero[pos_jugadores[J1][0]][pos_jugadores[J1][1]] = ' '; 
		pos_jugadores[J1][0] --;
		apunta[J1] = norte;
	    }
	    break;

	case ARRIBA:
	    if(tablero[pos_jugadores[J2][0] - 2][pos_jugadores[J2][1]] != 'H' &&
		    tablero[pos_jugadores[J2][0]-2][pos_jugadores[J2][1]] != CARACTERJ1){
		tablero[pos_jugadores[J2][0]][pos_jugadores[J2][1]] = ' ';
		pos_jugadores[J2][0] --;
		apunta[J2] = norte;
	    }
	    break;

	case KEY_DOWN:
	    if(tablero[pos_jugadores[J1][0] + 2][pos_jugadores[J1][1]] != 'H' &&
		    tablero[pos_jugadores[J1][0]+2][pos_jugadores[J1][1]] != CARACTERJ2){
		tablero[pos_jugadores[J1][0]][pos_jugadores[J1][1]] = ' ';
		pos_jugadores[J1][0] ++;
		apunta[J1] = sur;
	    }
	    break;

	case ABAJO:
	    if(tablero[pos_jugadores[1][0] + 2][pos_jugadores[1][1]] != 'H' &&
		    tablero[pos_jugadores[J2][0]+2][pos_jugadores[J2][1]] != CARACTERJ1){
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
//mueve a las balas
void mover_bala(arma bala[][MAXAMO], char tablero[][N], bool jugador_caido[], int puntuacion[]){

    for(int jugador=0; jugador<NJUGADORES; jugador++)
	for(int n_bala=0; n_bala<MAXAMO; n_bala++)
	    if (bala[jugador][n_bala].se_mueve){
		tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] = ' ';
		switch(bala[jugador][n_bala].direccion){
		    case este:
			bala[jugador][n_bala].col++;
			break;
		    case oeste:
			bala[jugador][n_bala].col--;
			break;
		    case norte:
			bala[jugador][n_bala].fila--;
			break;
		    case sur:
			bala[jugador][n_bala].fila++;
			break;
		}

		if(tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] == 'H')
		    bala[jugador][n_bala].se_mueve = false;
		else
		    if(tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] == '0' ||
			    tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] == '5'){
			bala[jugador][n_bala].se_mueve = false;
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
			tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] = '*';
	    }

}
void pintar_tablero(char tablero[][N], int respuesta, int puntuacion[2]){

    init_pair(VV, COLOR_GREEN, COLOR_GREEN);//asigna el color que se le dara
    init_pair(AV, COLOR_BLUE, COLOR_GREEN);
    init_pair(MV, COLOR_MAGENTA, COLOR_GREEN);
    init_pair(NV, COLOR_BLACK, COLOR_GREEN); 
    init_pair(BV, COLOR_WHITE, COLOR_GREEN);
    init_pair(RR, COLOR_RED, COLOR_RED);
    erase();
    for(int x=0; x<N; x++){
	for(int y=0; y<N; y++){

	    switch(tablero[x][y]){
		case ' ':
		    attron(COLOR_PAIR(VV));
		    printw("%c", tablero[x][y]);
		    attroff(COLOR_PAIR(VV));
		    break;
		case 'H':
		    attron(COLOR_PAIR(RR));
		    printw("%c", tablero[x][y]);
		    attroff(COLOR_PAIR(RR));

		    break;
		case CMUNICION:
		    attron(COLOR_PAIR(NV));
		    printw("%c", tablero[x][y]);
		    attroff(COLOR_PAIR(NV));

		    break;
		case CARACTERJ1:
		    attron(COLOR_PAIR(AV));
		    printw("%c", tablero[x][y]);
		    attroff(COLOR_PAIR(AV));
		    break;

		case CARACTERJ2:
		    attron(COLOR_PAIR(MV));
		    printw("%c", tablero[x][y]);
		    attroff(COLOR_PAIR(MV));
		    break;

		case ARMA:
		    attron(COLOR_PAIR(BV));
		    printw("%c", tablero[x][y]);
		    attroff(COLOR_PAIR(BV));
		    break;
	    }

	}
	printw("\n");
    }
    printw("J1-%i  J2-%i\n", puntuacion[J1], puntuacion[J2]);
    refresh();
}
void bucle_juego(){

    int respuesta,
	balas_disparadas[NJUGADORES],//cantidad de balas que se han disparado
	puntuacion[NJUGADORES];
    int pos_jugadores[NJUGADORES][COORDENADAS];
    char tablero[N][N];
    Direccion apunta[NJUGADORES];
    arma bala[NJUGADORES][MAXAMO];//balas de cada jugador
    bool jugador_caido[NJUGADORES];

    puntuacion[J1] = puntuacion[J2] = 0;

    do{
	//inicializan todo antes de la partida y despues de que un jugador muera
	balas_disparadas[J1] = balas_disparadas[J2] = 0;

	for(int m_jugador=0; m_jugador<NJUGADORES; m_jugador++)
	    for(int municion=0; municion<MAXAMO; municion++)
		bala[m_jugador][municion].se_mueve = false;

	pos_jugadores[J1][FILA] = pos_jugadores[J1][COLUMNA] = 5;
	pos_jugadores[J2][FILA] = pos_jugadores[J2][COLUMNA] = 10;

	for(int x=0; x<N; x++)
	    for(int y=0; y<N; y++)
		if(x == 0 || y == 0 || x == N-1 || y == N-1)
		    tablero[x][y] = 'H';
		else
		    tablero[x][y] = ' ';

	tablero[pos_jugadores[J1][FILA]][pos_jugadores[J1][COLUMNA]] = '0';
	tablero[pos_jugadores[J2][FILA]][pos_jugadores[J2][COLUMNA]] = '5';
	tablero[pos_jugadores[J1][FILA]][pos_jugadores[J1][COLUMNA]+1] =  
	    tablero[pos_jugadores[J2][FILA]][pos_jugadores[J2][COLUMNA]+1] = '=';
	apunta[J1] = apunta[J2] = este;


	jugador_caido[J1] = jugador_caido[J2] = false;
	pintar_tablero(tablero, respuesta, puntuacion);
	do{
	    timeout( 50 );
	    pintar_tablero(tablero, respuesta, puntuacion);
	    respuesta = getch();
	    mover_jugadores(respuesta, tablero, pos_jugadores, apunta, bala, balas_disparadas);
	    mover_bala(bala, tablero, jugador_caido, puntuacion);
	}while(respuesta != EXIT && jugador_caido[J1] != true && jugador_caido[J2] != true);
    }while(respuesta != EXIT);
}
int main(int argc, char *argv[]){
    initscr();
    raw();
    keypad(stdscr, TRUE); 
    noecho();
    start_color();

    comprobacion();
    bucle_juego();

    endwin(); 
    return EXIT_SUCCESS;
}
