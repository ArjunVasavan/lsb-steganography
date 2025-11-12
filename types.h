#ifndef TYPES_H // for preventing duplicate library adding
#define TYPES_H

#include <unistd.h>
/* User defined types */
typedef unsigned int uint; // so uint is unsigned itn i dont have to make the whole code clumsy

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;


#define print_sleep(fmt, ...) \
    do { \
        usleep(500000); \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)


typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
