pingas: main.c lodepng.c lodepng.h format.c format.h
	gcc main.c format.c lodepng.c -o pingas

all: pingas

install: pingas
	install pingas /usr/local/bin
