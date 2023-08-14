#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define MZ_STUB             "MZ"
#define MZ_STUB_ADDRESS     0x0

#define UPX_STUB            "UPX0"
#define UPX_STUB_ADDRESS    0x1f8

#define DATA_POINTER_80     0x144ac0
#define DATA_POINTER_81     0x226cfb
#define DATA_POINTER_82     0x226cfb

#define DATA_ADDRESS_80     0x1e8480
#define DATA_ADDRESS_81     0x39fbc4
#define DATA_ADDRESS_82     0x365244

typedef enum
{
    GM_UNKNOWN,
    GM_80,
    GM_81,
    GM_82,
} gm_version;

typedef unsigned int uint;

#define fail(format, ...)                       \
    {                                           \
        fprintf(stderr, format, ##__VA_ARGS__); \
        exit(EXIT_FAILURE);                     \
    }

void getRunner(const void *buffer, const void *size, uint dataoffset, FILE *stream);
void getData(const void *buffer, const void *size, uint dataoffset, FILE *stream);
bool fexist(const char *filename);
uint fcmp(FILE *file, const char *str, size_t size, long offset);
uint freaduint(FILE *file, long offset);
void fcopy(const char *source, const char *dest);
int systemf(const char *format, ...);
int prompt(const char *question);
long fsize(FILE *stream);