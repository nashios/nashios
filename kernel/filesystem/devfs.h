#pragma once

#define MINORBITS 20
#define MKDEV(ma, mi) (((ma) << MINORBITS) | (mi))

void devfs_init();
