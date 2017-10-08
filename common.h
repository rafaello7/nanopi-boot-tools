#ifndef COMMON_H
#define COMMON_H

void fatal(const char *fmt, ...);
void fatal_err(const char *fmt, ...);

const char *get_blkdev(void);

#endif /* COMMON_H */
