#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>

//Socket
#include <sys/types.h>
#include <sys/socket.h>
//Socket internet
#include <netinet/in.h>
#include <arpa/inet.h>
//Hilos
#include <pthread.h>

#include "comunicacion.h"

#define LENGTH 0x20
#define NJUGADORES 2
#define N 30
#define MAXBALAS 2

#define DISPARAR 32 

enum TDireccion { norte=1, sur, este, oeste};

struct TPosicion{
    int fila;
    int columna;
};

struct TBala{
    TDireccion direccion;
    struct TPosicion posicion;
    bool se_mueve;
};

struct TJugador{
    int respuesta; 
    int n_balas_disparadas;
    int puntuacion;
    struct TPosicion posicion;
    TDireccion direccion;
    bool abatido;
    struct TBala bala[MAXBALAS];
};

struct TDatos{
    int socket_fd;
    char origen[LENGTH];
    int clientes_esperando; //Contador que indica los clientes que espran un resultado de calculo.
    TJugador jugador[NJUGADORES];
    int *n_clientes;
};

struct TDatosCalculo{
    struct TDatos *datos_clientes;
    char tablero[N][N];
};

void error(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void enviar_datos(int fd, struct TJugador jugador[NJUGADORES]){
    char buffer[4]; 
    for(int i=0; i<NJUGADORES; i++){
	sprintf(buffer, "%d", jugador[i].posicion.fila);
	enviar(fd, buffer);
	sprintf(buffer, "%d", jugador[i].posicion.columna);
	enviar(fd, buffer);
	sprintf(buffer, "%d", jugador[i].direccion);
	enviar(fd, buffer);
	sprintf(buffer, "%d", jugador[i].abatido);
	enviar(fd, buffer);
	sprintf(buffer, "%d", jugador[i].puntuacion);
	enviar(fd, buffer);
    }
    for(int i=0; i<NJUGADORES; i++)
      for(int j=0; j<MAXBALAS; j++){
	sprintf(buffer, "%d", jugador[i].bala[j].se_mueve);
	enviar(fd, buffer);
	if(jugador[i].bala[j].se_mueve){
	  sprintf(buffer, "%d", jugador[i].bala[j].posicion.columna);
	  enviar(fd, buffer);
	  sprintf(buffer, "%d", jugador[i].bala[j].posicion.fila);
	  enviar(fd, buffer);
	}//Fin if
      }//Fin for bala
}

void recibir_datos(int fd, int *respuesta){
  char buffer[5];
  recibir(fd, buffer);
  *respuesta =  atoi(buffer);
}

void inicializar_juego(char tablero[N][N], struct TJugador jugador[NJUGADORES]){

  //Inicializa el tablero
  for(int x=0; x<N; x++)
    for(int y=0; y<N; y++)
      if(x == 0 || y == 0 || x == N-1 || y == N-1)
	tablero[x][y] = 'H';
      else
	tablero[x][y] = ' ';

  jugador[0].n_balas_disparadas = jugador[1].n_balas_disparadas = 0;
  for(int i=0; i<NJUGADORES; i++)
    for(int j=0;  j<MAXBALAS; j++)
      jugador[i].bala[j].se_mueve = false;
  jugador[0].posicion.fila = jugador[0].posicion.columna = 5;
  jugador[1].posicion.fila = jugador[1].posicion.columna = 10;
  tablero[jugador[0].posicion.fila][jugador[0].posicion.columna] = '0';
  tablero[jugador[1].posicion.fila][jugador[1].posicion.columna] = '5';
  tablero[jugador[0].posicion.fila][jugador[0].posicion.columna+1] =
    tablero[jugador[1].posicion.fila][jugador[1].posicion.columna+1] = '=';
  jugador[0].direccion = jugador[1].direccion = este;
  jugador[0].abatido = jugador[1].abatido = false;
}

void inicializa_puntuaciones(int *puntuacion){
  *puntuacion = 0;
}

void *calculo(void *args){
  struct TDatosCalculo *datos = (struct TDatosCalculo *) args;

  TDireccion direccion_anterior[2];
  while(1){
    while(datos->datos_clientes->clientes_esperando <= NJUGADORES);

    //borra el arma
    for(int borrar=0; borrar<NJUGADORES; borrar++)
      switch(direccion_anterior[borrar]){
	case este:
	  datos->tablero[datos->datos_clientes->jugador[borrar].posicion.fila]
	    [datos->datos_clientes->jugador[borrar].posicion.columna+1] = ' ';
	  break;
	case oeste:
	  datos->tablero[datos->datos_clientes->jugador[borrar].posicion.fila]
	    [datos->datos_clientes->jugador[borrar].posicion.columna-1] = ' ';
	  break;
	case norte:
	  datos->tablero[datos->datos_clientes->jugador[borrar].posicion.fila-1]
	    [datos->datos_clientes->jugador[borrar].posicion.columna] = ' ';
	  break;
	case sur:
	  datos->tablero[datos->datos_clientes->jugador[borrar].posicion.fila+1]
	    [datos->datos_clientes->jugador[borrar].posicion.columna] = ' ';
	  break;
      }

    for(int i=0; i<NJUGADORES; i++){
      switch(datos->datos_clientes->jugador[i].respuesta){

	case KEY_RIGHT:
	  if(datos->tablero[datos->datos_clientes->jugador[i].posicion.fila]
	      [datos->datos_clientes->jugador[i].posicion.columna+2] != 'H'){
	    datos->tablero[datos->datos_clientes->jugador[i].posicion.fila][datos->datos_clientes->jugador[i].posicion.columna]
	      = ' ';
	    datos->datos_clientes->jugador[i].posicion.columna ++;
	    datos->datos_clientes->jugador[i].direccion = este;
	  }
	  break;


	case KEY_LEFT:
	  if(datos->tablero[datos->datos_clientes->jugador[i].posicion.fila]
	      [datos->datos_clientes->jugador[i].posicion.columna-2] != 'H'){
	    datos->tablero[datos->datos_clientes->jugador[i].posicion.fila][datos->datos_clientes->jugador[i].posicion.columna]
	      = ' ';
	    datos->datos_clientes->jugador[i].posicion.columna --;
	    datos->datos_clientes->jugador[i].direccion = oeste;
	  }
	  break;

	case KEY_UP:
	  if(datos->tablero[datos->datos_clientes->jugador[i].posicion.fila-2]
	      [datos->datos_clientes->jugador[i].posicion.columna] != 'H'){
	    datos->tablero[datos->datos_clientes->jugador[i].posicion.fila][datos->datos_clientes->jugador[i].posicion.columna]
	      = ' ';
	    datos->datos_clientes->jugador[i].posicion.fila --;
	    datos->datos_clientes->jugador[i].direccion = norte;
	  }
	  break;

	case KEY_DOWN:
	  if(datos->tablero[datos->datos_clientes->jugador[i].posicion.fila+2]
	      [datos->datos_clientes->jugador[i].posicion.columna] != 'H'){
	    datos->tablero[datos->datos_clientes->jugador[i].posicion.fila][datos->datos_clientes->jugador[i].posicion.columna]
	      = ' ';
	    datos->datos_clientes->jugador[i].posicion.fila ++;
	    datos->datos_clientes->jugador[i].direccion = sur;
	  }
	  break;

	case DISPARAR:
	  int n = datos->datos_clientes->jugador[i].n_balas_disparadas % MAXBALAS;
	  if( !datos->datos_clientes->jugador[i].bala[n].se_mueve ){
	    datos->datos_clientes->jugador[i].bala[n].se_mueve = true;
	    datos->datos_clientes->jugador[i].bala[n].direccion = datos->datos_clientes->jugador[i].direccion;
	    switch(datos->datos_clientes->jugador[i].bala[n].direccion){
	      case norte:
		datos->datos_clientes->jugador[i].bala[n].posicion.columna 
		  = datos->datos_clientes->jugador[i].posicion.columna;
		datos->datos_clientes->jugador[i].bala[n].posicion.fila
		  = datos->datos_clientes->jugador[i].posicion.fila -1;
		break;
	      case sur:
		datos->datos_clientes->jugador[i].bala[n].posicion.columna 
		  = datos->datos_clientes->jugador[i].posicion.columna;
		datos->datos_clientes->jugador[i].bala[n].posicion.fila
		  = datos->datos_clientes->jugador[i].posicion.fila +1;
		break;
	      case este:
		datos->datos_clientes->jugador[i].bala[n].posicion.columna 
		  = datos->datos_clientes->jugador[i].posicion.columna +1;
		datos->datos_clientes->jugador[i].bala[n].posicion.fila
		  = datos->datos_clientes->jugador[i].posicion.fila;
		break;
	      case oeste:
		datos->datos_clientes->jugador[i].bala[n].posicion.columna 
		  = datos->datos_clientes->jugador[i].posicion.columna -1;
		datos->datos_clientes->jugador[i].bala[n].posicion.fila
		  = datos->datos_clientes->jugador[i].posicion.fila;
		break;
	    }
	    datos->datos_clientes->jugador[i].n_balas_disparadas ++;

	  }
	  break;
      }//Fin switch

      for(int n=0; n<MAXBALAS; n++)
	if (datos->datos_clientes->jugador[i].bala[n].se_mueve){
	  datos->tablero[datos->datos_clientes->jugador[i].bala[n].posicion.fila]
	    [datos->datos_clientes->jugador[i].bala[n].posicion.columna] = ' ';

	  switch(datos->datos_clientes->jugador[i].bala[n].direccion){
	    case este:
	      datos->datos_clientes->jugador[i].bala[n].posicion.columna ++;
	      break;
	    case oeste:
	      datos->datos_clientes->jugador[i].bala[n].posicion.columna --;
	      break;
	    case norte:
	      datos->datos_clientes->jugador[i].bala[n].posicion.fila --;
	      break;
	    case sur:
	      datos->datos_clientes->jugador[i].bala[n].posicion.fila ++;
	      break;
	  }

	  if(datos->tablero[datos->datos_clientes->jugador[i].bala[n].posicion.fila]
	      [datos->datos_clientes->jugador[i].bala[n].posicion.columna] == 'H')
	    datos->datos_clientes->jugador[i].bala[n].se_mueve = false;
	  else
	    if(datos->datos_clientes->jugador[i^1].posicion.fila == 
		    datos->datos_clientes->jugador[i].bala[n].posicion.fila && 
	            datos->datos_clientes->jugador[i^1].posicion.columna == 
		    datos->datos_clientes->jugador[i].bala[n].posicion.columna){

	      datos->datos_clientes->jugador[i].bala[n].se_mueve = false;
	      inicializar_juego(datos->tablero, datos->datos_clientes->jugador);
	      if(i == 0)
		datos->datos_clientes->jugador[0].puntuacion ++;
	      else
		datos->datos_clientes->jugador[1].puntuacion ++;
	    }
	    else
	      datos->tablero[datos->datos_clientes->jugador[i].bala[n].posicion.fila]
		[datos->datos_clientes->jugador[i].bala[n].posicion.columna] = '*';
	}

    }//Fin for jugadores de i

    direccion_anterior[0] = datos->datos_clientes->jugador[0].direccion;
    direccion_anterior[1] = datos->datos_clientes->jugador[1].direccion;

    //pinta el arma
    for(int pintar=0; pintar<NJUGADORES; pintar++)
      switch(direccion_anterior[pintar]){
	case este:
	  datos->tablero[datos->datos_clientes->jugador[pintar].posicion.fila]
	    [datos->datos_clientes->jugador[pintar].posicion.fila+1] = '=';
	  break;
	case oeste:
	  datos->tablero[datos->datos_clientes->jugador[pintar].posicion.fila]
	    [datos->datos_clientes->jugador[pintar].posicion.fila-1] = '=';
	  break;
	case norte:
	  datos->tablero[datos->datos_clientes->jugador[pintar].posicion.fila-1]
	    [datos->datos_clientes->jugador[pintar].posicion.fila] = '=';
	  break;
	case sur:
	  datos->tablero[datos->datos_clientes->jugador[pintar].posicion.fila+1]
	    [datos->datos_clientes->jugador[pintar].posicion.fila] = '=';
	  break;
      }

    datos->datos_clientes->clientes_esperando = 0;
  }


  pthread_detach(pthread_self());
}

void *jugador(void *datos){
  struct TDatos *dato = (struct TDatos *) datos;
  int this_fd = dato->socket_fd;
  char this_origen[LENGTH];
  strncpy(this_origen, dato->origen, LENGTH);
  int this_jugador = (*dato->n_clientes)-1;


  printf("Origen -> %s\n Jugador %i\n", this_origen, this_jugador);

  //Espera a que este el numero minimo de usuarios
  if(*dato->n_clientes < NJUGADORES)
    enviar(this_fd, "wait");
  while(*dato->n_clientes < NJUGADORES);
  enviar(this_fd, "no");
  pthread_detach(pthread_self());

  //Envia los datos iniciales
  enviar_datos(this_fd, dato->jugador);

  bool esperar = false;
  while(1){

    if(esperar){
      dato->clientes_esperando ++;
      while(dato->clientes_esperando >= NJUGADORES);
      esperar = false;
    }

    recibir_datos(this_fd, &dato->jugador[this_jugador].respuesta);
    enviar_datos(this_fd, dato->jugador);
    //if(dato->jugador[this_jugador].respuesta != -1)
    //printf("Presionaste %i \n", dato->jugador[this_jugador].respuesta);
    esperar = true;
  }

  close(this_fd);
  *dato->n_clientes --;
  printf("Se fue\n");
  return NULL;
}

int main(int argc, char *argv[]){

  int n_clientes_online = 0;

  //Variables necesarias para crear el servidor
  int socket_fd; //Descriptor de fichero
  struct sockaddr_in configuracion; //Estructura con la configuracion

  if(argc < 2){
    fprintf(stderr, "%s <puerto>\n", argv[0]);
    return EXIT_FAILURE;
  }

  //Crear el socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  //Configurar el socket
  configuracion.sin_family    = AF_INET;
  configuracion.sin_addr.s_addr = INADDR_ANY;
  configuracion.sin_port        = htons(atoi(argv[1]));
  bzero(configuracion.sin_zero, sizeof(configuracion.sin_zero));

  //Atar el servidor a la maquina
  if( bind(socket_fd, (struct sockaddr *) &configuracion, sizeof(struct sockaddr)) == -1 )
    error("Bind");

  //Poner el servidor a la escucha
  if( listen(socket_fd, NJUGADORES) )
    error("Listen");

  printf("Servidor a la escucha en el puerto %s\n\n", argv[1]);

  //Definimos las estructuras de datos e inicializamos los campos que lo requieren
  struct TDatos datos_clientes;
  datos_clientes.n_clientes = &n_clientes_online;
  datos_clientes.clientes_esperando = 0;
  struct TDatosCalculo datos_calculo;
  datos_calculo.datos_clientes = &datos_clientes;

  inicializar_juego(datos_calculo.tablero, datos_clientes.jugador);
  inicializa_puntuaciones(&datos_clientes.jugador[0].puntuacion);
  inicializa_puntuaciones(&datos_clientes.jugador[1].puntuacion);

  //Creamos el hilo calcular
  pthread_t id_calculo;
  pthread_create(&id_calculo, NULL, &calculo, &datos_calculo);
  //...

  while(1){
    //ID de los hilos
    pthread_t id_hilo;
    //Configuracion del cliente
    struct sockaddr_in cliente;
    socklen_t cliente_longitud = (socklen_t) sizeof(struct sockaddr);
    int cliente_fd;

    if( (cliente_fd = accept(socket_fd, (struct sockaddr *) &cliente, &cliente_longitud)) == -1 )
      error("Accept");

    datos_clientes.socket_fd = cliente_fd;
    strncpy(datos_clientes.origen, inet_ntoa(cliente.sin_addr), LENGTH);
    datos_clientes.origen[N-1] = '\0';

    n_clientes_online ++;

    pthread_create(&id_hilo, NULL, &jugador, &datos_clientes);
  }

  printf("Servidor cerrado \n\n");
  close(socket_fd);
  return EXIT_SUCCESS;
}
