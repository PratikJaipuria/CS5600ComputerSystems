myprog:	myprog.o libckpt.so
	gcc -g -o myprog myprog.o

libckpt.so: libckpt.o
	gcc libckpt.o -shared -o libckpt.so

libckpt.o:
	gcc -c -fPIC myprog.c -o libckpt.o	

clean:
	rm myprog.o libckpt.o