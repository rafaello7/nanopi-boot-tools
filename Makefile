all:: nano-ubootenv nano-blembed

nano-ubootenv: ubootenv.o common.o
	gcc ubootenv.o common.o -o nano-ubootenv

nano-blembed: blembed.o common.o
	gcc blembed.o common.o -o nano-blembed

.c.o:
	gcc -O -c -Wall $<

clean:
	rm -f nano-ubootenv nano-blembed *.o

deb:
	dpkg-buildpackage -b -uc
