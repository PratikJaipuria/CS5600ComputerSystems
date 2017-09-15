all: check

default: check

ckpt.o: libckpt.c
	gcc -c -Wall -Werror -fpic -o ckpt.o libckpt.c

libckpt.so: ckpt.o
	gcc -shared -o libckpt.so ckpt.o


myprog.o:	myprog.c
	gcc -c -Wall -Werror -fpic -o myprog.o myprog.c

myprog: myprog.o
	gcc -g -o myprog myprog.o

check:	libckpt.so myprog
	(sleep 3 && kill -12 `pgrep -n myprog` && sleep 2 && pkill -9 myprog) & 
	LD_PRELOAD=`pwd`/libckpt.so ./myprog
	
clean:
	rm myprog.o ckpt.o libckpt.so ckpt myprog

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar	