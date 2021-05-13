parell: main.o threads.o lista.o lista_th.o
	gcc -o parell main.o threads.o lista.o lista_th.o -lpthread -fopenmp

main.o: ../main.c ../main.h -lpthread
	gcc -c ../main.c -fopenmp
	
threads.o: ../libs/lib_thread/threads.c ../libs/lib_thread/threads.h -lpthread
	gcc -c ../libs/lib_thread/threads.c -fopenmp

lista_th.o: ../libs/lib_thread/lista_th.c ../libs/lib_thread/lista_th.h
	gcc -c ../libs/lib_thread/lista_th.c 

lista.o: ../libs/lib_lista_int/lista.c ../libs/lib_lista_int/lista.h
	gcc -c ../libs/lib_lista_int/lista.c


