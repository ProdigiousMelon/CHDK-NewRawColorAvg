/*	ppmio.c

	Code to copy a P6 ppm file

	1999 by H. Dietz
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

#include <fcntl.h>
#include <sys/mman.h>


unsigned char *
read_P6(register char *filename,
int *xdim,
int *ydim,
int *maxval)
{
	/* The following is a fast and sloppy way to
	   read a color raw PPM (P6) image file
	*/
	register int fd;
	register int fsize;
	register unsigned char *map;
	register unsigned char *p;

	/* First, open the file... */
	if ((fd = open(filename, O_RDONLY)) < 0) {
		return((unsigned char *) 0);
	}

	/* Read size and map the whole file... */
	fsize = lseek(fd, ((off_t) 0), SEEK_END);
	map = ((unsigned char *)
	       mmap(0,		/* Put it anywhere */
		    fsize,	/* Map the whole file */
		    PROT_READ,	/* Read only */
		    MAP_SHARED,	/* Not just for me */
		    fd,		/* The file */
		    0));	/* Right from the start */
	if (map == ((unsigned char *) -1)) {
		close(fd);
		return((unsigned char *) 0);
	}

	/* File should now be mapped; read magic value */
	p = map;
	if (*(p++) != 'P') goto ppm_exit;
	switch (*(p++)) {
	case '6':
		break;
	default:
		goto ppm_exit;
	}

#define	Eat_Space \
	while ((*p == ' ') || \
	       (*p == '\t') || \
	       (*p == '\n') || \
	       (*p == '\r') || \
	       (*p == '#')) { \
		if (*p == '#') while (*(++p) != '\n') ; \
		++p; \
	}

	Eat_Space;		/* Eat white space and comments */

#define	Get_Number(n) \
	{ \
		register int charval = *p; \
 \
		if ((charval < '0') || (charval > '9')) goto ppm_exit; \
 \
		n = (charval - '0'); \
		charval = *(++p); \
		while ((charval >= '0') && (charval <= '9')) { \
			n *= 10; \
			n += (charval - '0'); \
			charval = *(++p); \
		} \
	}

	Get_Number(*xdim);	/* Get image width */

	Eat_Space;		/* Eat white space and comments */
	Get_Number(*ydim);	/* Get image width */

	Eat_Space;		/* Eat white space and comments */
	Get_Number(*maxval);	/* Get image max value */

	/* Should be 8-bit binary after one whitespace char... */
	if (*maxval > 255) {
ppm_exit:
		close(fd);
		munmap(map, fsize);
		return((unsigned char *) 0);
	}
	if ((*p != ' ') &&
	    (*p != '\t') &&
	    (*p != '\n') &&
	    (*p != '\r')) goto ppm_exit;

	/* Here we are... next byte begins the 24-bit data */
	return(p + 1);

	/* Notice that we never clean-up after this:

	   close(fd);
	   munmap(map, fsize);

	   However, this is relatively harmless;
	   they will go away when this process dies.
	*/
}

#undef	Eat_Space
#undef	Get_Number


int
write_P6(register char *filename,
register char *comment,
register int xdim,
register int ydim,
register int maxval,
register unsigned char *data)
{
	register int fd;
	register int len;
	char buf[512];

	/* First, open the file... */
	fd = open(filename, (O_WRONLY | O_CREAT), 0644);
	if (fd < 0) {
		return(1);
	}

	/* Then write the header... */
	sprintf(&(buf[0]),
		"P6\n#%s\n%d %d\n%d\n",
		(comment ?
		 comment :
		 filename),
		xdim,
		ydim,
		maxval);
	len = strlen(&(buf[0]));
	if (write(fd, &(buf[0]), len) != len) {
		close(fd);
		return(2);
	}

	/* Then write the data... */
	len = (xdim * ydim * 3);
	if (write(fd, data, len) != len) {
		close(fd);
		return(3);
	}

	/* All looks good */
	close(fd);
	return(0);
}
