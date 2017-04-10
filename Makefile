CC=gcc
CFLAGS=-I

lab4: pushing_pixels.c ppmio.c
	gcc -o HDR pushing_pixels.c ppmio.c
