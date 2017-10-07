ubootenv: ubootenv.c
	gcc -O -Wall ubootenv.c -o ubootenv

clean:
	rm -f ubootenv

deb:
	dpkg-buildpackage -b -uc
