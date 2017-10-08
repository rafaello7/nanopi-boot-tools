#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include <mntent.h>


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
			/* NanoPC-T3: determine device using /boot location */
			struct mntent *ment;
			FILE *fp = fopen("/etc/fstab", "r");

			if( fp != NULL ) {
				fprintf(stderr, "determinig boot device using /etc/fstab\n");
				while( (ment = getmntent(fp)) != NULL ) {
					if( !strcmp(ment->mnt_dir, "/") ||
							!strcmp(ment->mnt_dir, "/boot") )
					{
						if( !strncmp(ment->mnt_fsname, blkdev_sd,
									strlen(blkdev_sd)) )
							blkdev = blkdev_sd;
						else if( !strncmp(ment->mnt_fsname, blkdev_emmc,
									strlen(blkdev_emmc)) )
							blkdev = blkdev_emmc;
						if( !strcmp(ment->mnt_dir, "/boot") )
							break;
					}
				}
				fclose(fp);
			}
		}
		if( blkdev == NULL ) {
			fprintf(stderr, "unable to determine boot device\n");
			exit(1);
		}
	}
	return blkdev;
}

