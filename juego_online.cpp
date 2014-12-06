//#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <stdio_ext.h>

//Libreriras para la conexion online
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "comunicacion.h"

#define N 30 //cantidad de celdas del mapa
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

struct TArma {
    Direccion direccion;
    int col, fila;
    bool se_mueve;
};

void error(const char *mensaje){
    endwin(); //Quitar ncurses
    perror(mensaje); //Imprimir error
    exit(EXIT_FAILURE);
}

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

void conexion(int *socket_fd, const char* direccion, int puerto){
    struct sockaddr_in servidor;
    struct hostent *he;

    if( (*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	error("Socket");

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    if( (he = gethostbyname(direccion)) == NULL )
	error("DNS");
    servidor.sin_addr = *((struct in_addr *) he->h_addr);
    bzero(&servidor.sin_zero, sizeof(servidor.sin_zero));

    if( connect(*socket_fd, (struct sockaddr *) &servidor, sizeof(struct sockaddr)) == -1)
	error("Conexion");
}

void esperar_jugadores(int socket_fd){
    char buffer[1000];
    recibir(socket_fd, buffer);
    if(!strcmp(buffer, "wait")){
	printw("Esperando mas usuarios...\n");
	refresh();
    }
    while(!strcmp(buffer, "wait"))
	recibir(socket_fd, buffer);
}

void enviar_datos(int fd, int respuesta){
    char buffer[100]; 
    sprintf(buffer, "%d", respuesta);
    enviar(fd, buffer);
}

void recibir_datos(int fd, int pos_jugadores[][COORDENADAS], Direccion apunta[], bool jugador_caido[], TArma bala[][MAXAMO], int puntuacion[NJUGADORES]){
    char buffer[5];


    //Actualiza a los jugadores
    for(int jugador=0; jugador<NJUGADORES; jugador++){
	for(int pos=0; pos<COORDENADAS; pos++){
	    recibir(fd, buffer);
	    pos_jugadores[jugador][pos] = atoi(buffer);
	}
	recibir(fd, buffer);
	apunta[jugador] = (Direccion) atoi(buffer);
	recibir(fd, buffer);
	jugador_caido[jugador] = (bool) atoi(buffer);
	recibir(fd, buffer);
	puntuacion[jugador] = atoi(buffer);
    }

    //Actualiza las balas
    for(int jugador=0; jugador<NJUGADORES; jugador++)
	for(int n_bala=0; n_bala<MAXAMO; n_bala++){
	    recibir(fd, buffer);
	    bala[jugador][n_bala].se_mueve = atoi(buffer);
	    if(bala[jugador][n_bala].se_mueve){
		recibir(fd, buffer);
		bala[jugador][n_bala].col  = atoi(buffer);
		recibir(fd, buffer);
		bala[jugador][n_bala].fila = atoi(buffer);
	    }
	}

}

void vaciar_tablero(char tablero[N][N], int pos_jugadores[NJUGADORES][COORDENADAS], 
	Direccion apunta[NJUGADORES], TArma bala[NJUGADORES][MAXAMO]){

    //Limpia a los jugadores
    for(int jugador=0; jugador<NJUGADORES; jugador++)
	tablero[pos_jugadores[jugador][0]][pos_jugadores[jugador][1]] = ' ';

    //Limpia el TArma
    for(int pintar_arma=0; pintar_arma<2; pintar_arma++)
	switch(apunta[pintar_arma]){
	    case este:
		tablero[pos_jugadores[pintar_arma][0]][pos_jugadores[pintar_arma][1]+1] = ' ';
		break;
	    case oeste:
		tablero[pos_jugadores[pintar_arma][0]][pos_jugadores[pintar_arma][1]-1] = ' ';
		break;
	    case norte:
		tablero[pos_jugadores[pintar_arma][0]-1][pos_jugadores[pintar_arma][1]] = ' ';
		break;
	    case sur:
		tablero[pos_jugadores[pintar_arma][0]+1][pos_jugadores[pintar_arma][1]] = ' ';
		break;
	}

    //Limpia las balas
    for(int jugador=0; jugador<NJUGADORES; jugador++)
	for(int n_bala=0; n_bala<MAXAMO; n_bala++)
	    if (bala[jugador][n_bala].se_mueve)
		tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] = ' ';
}

void rellenar_tablero(char tablero[N][N], int pos_jugadores[NJUGADORES][COORDENADAS], 
	Direccion apunta[NJUGADORES], TArma bala[NJUGADORES][MAXAMO]){

    //Posiciona a los jugadores
    tablero[pos_jugadores[1][0]][pos_jugadores[1][1]] = '5';
    tablero[pos_jugadores[0][0]][pos_jugadores[0][1]] = '0';

    //Posiciona el TArma
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

    //Posiciona las balas
    for(int jugador=0; jugador<NJUGADORES; jugador++)
	for(int n_bala=0; n_bala<MAXAMO; n_bala++)
	    if (bala[jugador][n_bala].se_mueve)
		tablero[bala[jugador][n_bala].fila][bala[jugador][n_bala].col] = '*';

}

void pintar_tablero(char tablero[][N], int puntuacion[2]){

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

void bucle_juego(int socket_fd){

    int respuesta,
	balas_disparadas[NJUGADORES],//cantidad de balas que se han disparado
	puntuacion[NJUGADORES];
    int pos_jugadores[NJUGADORES][COORDENADAS];
    char tablero[N][N];
    Direccion apunta[NJUGADORES];
    TArma bala[NJUGADORES][MAXAMO];//balas de cada jugador
    bool jugador_caido[NJUGADORES];

    do{
	for(int x=0; x<N; x++)
	    for(int y=0; y<N; y++)
		if(x == 0 || y == 0 || x == N-1 || y == N-1)
		    tablero[x][y] = 'H';
		else
		    tablero[x][y] = ' ';

	recibir_datos(socket_fd, pos_jugadores, apunta, jugador_caido, bala, puntuacion);
	rellenar_tablero(tablero, pos_jugadores, apunta, bala);
	pintar_tablero(tablero, puntuacion);
	do{
	    timeout( 1 );
	    pintar_tablero(tablero, puntuacion);
	    respuesta = getch();
	    __fpurge(stdin);
	    enviar_datos(socket_fd, respuesta);
	    vaciar_tablero(tablero, pos_jugadores, apunta, bala);
	    recibir_datos(socket_fd, pos_jugadores, apunta, jugador_caido, bala, puntuacion);
	    rellenar_tablero(tablero, pos_jugadores, apunta, bala);
	}while(respuesta != EXIT && jugador_caido[J1] != true && jugador_caido[J2] != true);
    }while(respuesta != EXIT);
}

int main(int argc, char *argv[]){
    int socket_fd;
    if(argc < 3){
	fprintf(stderr, "%s <ip> <puerto>\n", argv[0]);
	return EXIT_FAILURE;
    }


    initscr();
    raw();
    keypad(stdscr, TRUE); 
    noecho();
    start_color();

    comprobacion();
    conexion(&socket_fd, argv[1], atoi(argv[2]));
    esperar_jugadores(socket_fd);

    bucle_juego(socket_fd);

    close(socket_fd);
    endwin(); 
    return EXIT_SUCCESS;
}
