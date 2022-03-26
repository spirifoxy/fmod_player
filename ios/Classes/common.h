#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#include "fmod.h"

#define ERR_BUF_SIZE 1024
bool ERRCHECK_fn(FMOD_RESULT result, char *error, size_t len, const char *file, int line);
#define ERRCHECK(_result, _error) ERRCHECK_fn(_result, _error, sizeof(_error), __FILE__, __LINE__)

#endif
