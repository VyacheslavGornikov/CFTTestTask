#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define MAX_ARRAY_SIZE 100000
#define MAX_MODE_BITS 3
#define MAX_PATH_LEN 256

typedef struct StatData
{
    long id;
    int count;
    float cost;
    unsigned int primary:1;
    unsigned int mode:3;
} StatData;

#endif /* _COMMON_H_ */