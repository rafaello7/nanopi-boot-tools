#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include <sys/sysmacros.h>
#include <sys/stat.h>


void fatal(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(1);
}

void fatal_err(const char *fmt, ...)
{
	int err = errno;
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	if( err )
		fprintf(stderr, ": %s", strerror(err));
	fprintf(stderr, "\n");
	exit(1);
}


const char *get_blkdev(void)
{
	static const char blkdev_sd[] = "/dev/mmcblk2";
	static const char blkdev_emmc[] = "/dev/mmcblk0";
	static const char *blkdev = NULL;

	if( blkdev == NULL ) {
		if( access(blkdev_emmc, F_OK) == -1 ) {
			/* NanoPi M3 - no emmc */
			blkdev = blkdev_sd;
		}else if( access(blkdev_sd, F_OK) == -1 ) {
			/* NanoPC-T3 without SD card inserted */
			blkdev = blkdev_emmc;
		}else{
			/* NanoPC-T3: determine boot device by /boot location */
			struct stat st, stdev;
			char blkpart[40];
			unsigned boot_minor;

			if( stat("/boot", &st) < 0 )
				fatal_err("stat on /boot fail");
			boot_minor = minor(st.st_dev);
			sprintf(blkpart, "%sp%d", blkdev_emmc, boot_minor);
			if( stat(blkpart, &stdev) == 0 && st.st_dev == stdev.st_rdev ) {
				blkdev = blkdev_emmc;
			}else{
				sprintf(blkpart, "%sp%d", blkdev_sd, boot_minor);
				if( stat(blkpart, &stdev) == 0 && st.st_dev == stdev.st_rdev ) {
					blkdev = blkdev_sd;
				}else
					fatal("unable to determine boot device\n");
			}
			fprintf(stderr, "guess boot device: %s\n",
					blkdev == blkdev_emmc ? "EMMC" : "SD");
		}
	}
	return blkdev;
}

