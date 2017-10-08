#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "common.h"

enum {
	ENV_OFFSET = 512 * 1024,
	ENV_SIZE = 4096	// environment size fro u-boot on NanoPi
};

static void crccalc(const char *data, unsigned len, char *crcbuf)
{
	unsigned crc = 0xffffffffL;
	unsigned c, k;

	while( len-- ) {
		c = (crc ^ *data++) & 255;
		for (k = 0; k < 8; k++)
			c = (c >> 1) ^ (c&1 ? 0xedb88320L : 0);
		crc = c ^ (crc >> 8);
	}
	crc ^= 0xffffffffL;
	// store result little-endian
	crcbuf[0] = crc;
	crcbuf[1] = crc >> 8;
	crcbuf[2] = crc >> 16;
	crcbuf[3] = crc >> 24;
}

static void readenv(const char *outfname)
{
	char buf[ENV_SIZE], crcbuf[4];
	FILE *fp;
	int off;
	const char *blkdev = get_blkdev();

	if( (fp = fopen(blkdev, "r")) == NULL )
		fatal_err("can't open %s for reading", blkdev);
	if( fseek(fp, ENV_OFFSET, SEEK_SET) )
		fatal_err("%s lseek fail", blkdev);
	if( fread(buf, sizeof(buf), 1, fp) != 1 )
		fatal_err("%s read fail", blkdev);
	fclose(fp);
	if( outfname ) {
		if( (fp = fopen(outfname, "w")) == NULL )
			fatal_err("unable to open %s for writing", outfname);
	}else
		fp = stdout;
	for(off = 4; off < ENV_SIZE; ++off) {
		if(buf[off])
			fputc(buf[off], fp);
		else if( off == 0 || buf[off-1] == 0 )
			break;
		else
			fputc('\n', fp);
	}
	if( outfname )
		fclose(fp);
	crccalc(buf+4, ENV_SIZE-4, crcbuf);
	if( memcmp(buf, crcbuf, 4) )
		fprintf(stderr, "\nWARN: bad crc\n\n");
}

static void writeenv(const char *infname)
{
	char buf[ENV_SIZE];
	FILE *fp;
	int c, len = 4;
	const char *blkdev = get_blkdev();

	if( infname ) {
		if( (fp = fopen(infname, "r")) == NULL )
			fatal_err("unable to open %s for reading", infname);
	}else
		fp = stdin;
	while( (c = fgetc(fp)) != EOF ) {
		if( c == '\r' )
			continue;
		if( len == ENV_SIZE )
			fatal("fatal: environment too long\n");
		if( c == '\n' ) {
			if( len == 0 || buf[len-1] == '\0' )
				continue;
			c = '\0';
		}
		buf[len++] = c;
	}
	if( infname )
		fclose(fp);
	memset(buf + len, 0, ENV_SIZE - len);
	crccalc(buf+4, ENV_SIZE-4, buf);
	if( (fp = fopen(blkdev, "r+")) == NULL )
		fatal_err("can't open %s for writing", blkdev);
	if( fseek(fp, ENV_OFFSET, SEEK_SET) )
		fatal_err("%s lseek fail", blkdev);
	if( fwrite(buf, sizeof(buf), 1, fp) != 1 )
		fatal_err("%s write fail", blkdev);
	fclose(fp);
}

int main(int argc, char *argv[])
{
	if( argc == 1 || (argv[1][0] != 'r' && argv[1][0] != 'w') ) {
		printf("\nUtility to update u-boot environment on SD card\n\n");
		printf("usage:\n");
		printf("   nano-ubootenv r [<outfile>]\t- print current "
				"u-boot environment\n");
		printf("   nano-ubootenv w [<infile>]\t- set new u-boot environment\n");
		printf("\n");
		return 0;
	}
	if( argv[1][0] == 'r' )
		readenv(argv[2]);
	else
		writeenv(argv[2]);
	return 0;
}

