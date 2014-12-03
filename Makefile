servidor: servidor.o comunicacion.o
	    g++ servidor.o comunicacion.o -o servidor -lncurses -lpthread

juego_online: juego_online.o comunicacion.o
	g++ juego_online.o comunicacion.o -o juego_online -lncurses

clean: 
	rm -rf servidor.o comunicacion.o servidor.o servidor juego_online

