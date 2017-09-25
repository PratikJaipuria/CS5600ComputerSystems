master:	master.o worker
	gcc -o master master.o

master.o:	master.c
	gcc -c master.c -o master.o 

worker:	worker.o
	gcc -o worker worker.o -lm

worker.o:	worker.c
	gcc -c worker.c -o worker.o -lm

clean:
	rm master.o master worker.o worker
