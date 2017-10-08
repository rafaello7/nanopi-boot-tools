#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "common.h"


static void write32LE(char *addr, unsigned data)
{
	addr[0] = (unsigned char)(data & 0xff);
	addr[1] = (unsigned char)((data >> 8) & 0xff);
	addr[2] = (unsigned char)((data >> 16) & 0xff);
	addr[3] = (unsigned char)((data >> 24) & 0xff);
}

static void bl_embed(const char *fname)
{
	char buf[512 * 1024];
	FILE *fp;
	int off = 512, len, embed_off;
	const char *blkdev = get_blkdev();

	if( (fp = fopen(fname, "r")) == NULL )
		fatal_err("unable to open %s for reading", fname);
	if( (len = fread(buf + off, 1, sizeof(buf) - off, fp)) < 0 )
		fatal_err("fread");
	if( len < 512 )
		fatal("not a bootloader file");

	if( len < 65536 && ! memcmp(buf + off + 508, "NSIH", 4) ) {
		/* bl1 image */
		if( len > 32768 - 512 )
			fatal("bl1 image too big");

		// fix MMC drive number in bl1 NSIH header
		if( !strcmp(blkdev, "/dev/mmcblk0") )
			buf[off + 0x50] = 2;			// MMC drive number
		embed_off = 512;
	}else{
		/* u-boot image */
		if( off + len > 480 * 1024 )
			fatal("u-boot image too big");

		// add NSIH header
		memset(buf, 0, 512);
		write32LE(buf + 0x44, 1024 * (len/1024 + 2)); // load size
		write32LE(buf + 0x48, 0x43bffe00);  // load address
		write32LE(buf + 0x4c, 0x43c00000);  // launch address
		buf[0x57] = 3;						// boot method (SDMMC)
		memcpy(buf + 0x1fc, "NSIH", 4);     // signature

		len += off;
		off = 0;
		embed_off = 32768;
	}

	if( (fp = fopen(blkdev, "r+")) == NULL )
		fatal_err("can't open %s for writing", blkdev);
	if( fseek(fp, embed_off, SEEK_SET) )
		fatal_err("%s lseek fail", blkdev);
	if( fwrite(buf + off, len, 1, fp) != 1 )
		fatal_err("%s write fail", blkdev);
	fclose(fp);
	printf("installed on %s at offset %d, %d bytes\n", blkdev, embed_off, len);
}

int main(int argc, char *argv[])
{
	if( argc == 1 ) {
		printf("\nUtility to embed bl1 or u-boot boot loader on SD card\n\n");
		printf("usage:\n");
		printf("   nano-blembed <file>\n");
		printf("\n");
		return 0;
	}
	bl_embed(argv[1]);
	return 0;
}

