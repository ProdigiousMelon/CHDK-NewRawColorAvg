/*
High dynamic range processing program

Byron Giles
*/
#include <stdio.h>

extern unsigned char *
read_P6(register char *filename,
int *xdim,
int *ydim,
int *maxval);

extern int
write_P6(register char *filename,
register char *comment,
register int xdim,
register int ydim,
register int maxval,
register unsigned char *data);

int main(register int argc, register char **argv){
	register unsigned char *p1, *p2;
	int xdim, ydim, maxval;
	unsigned int pixelAverage;
	unsigned char charPixelAverage;
	/* Read the input */
	p1 = read_P6(argv[1],
		    &xdim,
		    &ydim,
		    &maxval);
	p2 = read_P6(argv[2],
		    &xdim,
		    &ydim,
		    &maxval);

	//sumation
	unsigned long numBytes = xdim * ydim * 3;
	unsigned char* pOut = (unsigned char*)calloc(numBytes, sizeof(char));
	int i;
	for (i = 0; i < numBytes; i++){
		pixelAverage = (p1[i] + p2[i])/2;
		charPixelAverage = pixelAverage;
		pOut[i] = charPixelAverage;
	}

	if (p1 == 0) {
		fprintf(stderr,
			"%s: could not read %s\n",
			argv[0],
			argv[1]);
		exit(2);
	}



	/* Write the output */
	if (write_P6(argv[3],
		     "just a test",
		     xdim,
		     ydim,
		     maxval,
		     pOut)) {
		fprintf(stderr,
			"%s: could not write %s\n",
			argv[0],
			argv[2]);
		exit(2);
	}
	return 0;
}
