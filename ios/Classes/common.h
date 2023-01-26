#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#include <stddef.h>
#include <stdio.h>

#include <fmod.h>
#include <fmod_errors.h>

#define bool int
#define true 1
#define false 0

#define ERR_BUF_SIZE 1024
bool ERRCHECK_fn(FMOD_RESULT result, char *error, size_t len, const char *file, int line);
#define ERRCHECK(_result, _error) ERRCHECK_fn(_result, _error, sizeof(_error), __FILE__, __LINE__)

#endif
