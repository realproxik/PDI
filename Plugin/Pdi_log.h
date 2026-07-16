#ifndef PDI_LOG_H
#define PDI_LOG_H

#include <stdio.h>

#define PDI_LOG_INFO(fmt, ...)  fprintf(stdout, "[PDI][INFO] " fmt "\n", ##__VA_ARGS__)
#define PDI_LOG_WARN(fmt, ...)  fprintf(stderr, "[PDI][WARN] " fmt "\n", ##__VA_ARGS__)
#define PDI_LOG_ERROR(fmt, ...) fprintf(stderr, "[PDI][ERROR] " fmt "\n", ##__VA_ARGS__)

#endif /* PDI_LOG_H */
