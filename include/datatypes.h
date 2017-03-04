#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdbool.h>
#include <stddef.h>

//Pass -D TRACE flag to GCC to turn on debug messages
#ifdef TRACE
#define trace(fmt, ...) \
            do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
#else
#define trace(fmt, ...) ((void)0)
#endif


typedef unsigned int uint32;
typedef int int32;
typedef long long int64;
typedef unsigned long long uint64;


#endif // DATATYPES_H
