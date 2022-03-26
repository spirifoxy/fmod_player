#include <cstdio>

#include "common.h"
#include "fmod_errors.h"

bool ERRCHECK_fn(FMOD_RESULT result, char *error, size_t len, const char *file, int line) {
    if (result == FMOD_OK) {
        return false;
    }

    const char *errorString = FMOD_ErrorString(result);
    snprintf(error, len, "ERROR: %s(%d): FMOD error %d - %s", file, line, result, errorString);
    return true;
}
