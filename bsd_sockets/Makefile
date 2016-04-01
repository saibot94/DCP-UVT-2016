all: bsd_sockets.o handler.o
	gcc -o echo_server bsd_sockets.o handler.o
handler.o:
	gcc -c -o handler.o -Wall handler.c
bsd_sockets.o:
	gcc -c -o bsd_sockets.o -Wall bsd_sockets.c

clean:
	rm ./*.o echo_server
