all: check

default: check

ckpt.o: libckpt.c
	gcc -fno-stack-protector -O0 -c -Wall -Werror -fpic -o ckpt.o libckpt.c

clean-ckpt:
	rm -rf ckpt

libckpt.so: ckpt.o
	gcc -fno-stack-protector -O0 -shared -o libckpt.so ckpt.o

myprog.o:	myprog.c
	gcc -fno-stack-protector -O0 -c -fpic -o myprog.o myprog.c

myprog: myprog.o
	gcc -fno-stack-protector -O0 -g -o myprog myprog.o

check:	libckpt.so myprog restart
	(sleep 3 && kill -12 `pgrep -n myprog` && sleep 2 && pkill -9 myprog) & 
	LD_PRELOAD=`pwd`/libckpt.so ./myprog
	(sleep 2 &&  pkill -9 restart) &
	make res

restart: restart.c
	gcc -fno-stack-protector -O0 -g -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o restart restart.c

res: 	restart
	./restart ckpt
	
clean:
	rm myprog.o ckpt.o libckpt.so ckpt myprog restart

build:	libckpt.so restart myprog

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar	