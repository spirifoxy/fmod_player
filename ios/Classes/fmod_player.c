#include <stdio.h>
#include <stdlib.h>

#include <fmod.h>

#include "common.h"
#include "native_player.h"

FMOD_RESULT F_CALLBACK myread(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata) {
    if (!handle) {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (bytesread) {
        // https://www.compuphase.com/mp3/mp3loops.htm
        int numread;
        int total = 0;
        while (sizebytes > 0) {
          numread = fread(buffer, 1, sizebytes, (FILE *)handle);
          sizebytes -= numread;
          total += numread;
          buffer = (unsigned char*)buffer + numread;
          // if there is more to read, wrap around and continue
          if (sizebytes > 0)
            fseek((FILE *)handle, 0, SEEK_SET);
        }

        *bytesread = total;
        if (*bytesread < sizebytes) {
            return FMOD_ERR_FILE_EOF;
        }
    }

    return FMOD_OK;
}

void initialize(void (*printCallback)(char *, bool)) {
    if (logger == NULL) {
        logger = printCallback;
    }
    if (finst != NULL) {
        commonLogger((char*)"WARN: was already initialized, returning");
        return;
    }

    FMOD_RESULT    result;
    char           error[ERR_BUF_SIZE];

    finst = (sfmod *)malloc(sizeof(sfmod));
    result = FMOD_System_Create(&finst->system, FMOD_VERSION);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = FMOD_System_SetDSPBufferSize(finst->system, 1024, 4);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = FMOD_System_Init(finst->system, 16, FMOD_INIT_NORMAL, NULL);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = FMOD_System_SetFileSystem(finst->system, 0, 0, myread, 0, 0, 0, 2048);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    commonLogger((char*)"INFO: native player Initialized");
}

void setHVolume(void *handle, double volume) {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];

    fplayer *fp = (fplayer *) handle;

    if (fp == NULL) {
        logger((char*)"ERROR: attempt to change volume while not initialized", true);
        return;
    }

    fp->volume = (float)volume;
    result = FMOD_Channel_SetVolume(fp->channel, fp->volume);
    if (ERRCHECK(result, error)) {
        logger(error, true);
        return;
    }

    result = FMOD_System_Update(finst->system);
    if (ERRCHECK(result, error)) {
        logger(error, true);
        return;
    }
}

void pauseH(void *handle) {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];

    fplayer *fp = (fplayer *)handle;

    if (fp == NULL) { // means memory was already cleared
        errorLogger((char*)"ERROR: attempt to pause while not initialized");
        return;
    }

    if (fp->channel == NULL) {
        errorLogger((char*)"ERROR: pause called while channel is null");
        return;
    }

    result = FMOD_Channel_SetPaused(fp->channel, true);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = FMOD_System_Update(finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}

void *stopH(void *handle) {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];

    fplayer *fp = (fplayer *)handle;

    if (fp == NULL) {
        errorLogger((char*)"ERROR: attempt to stop while not initialized");
        return NULL;
    }

    if (fp->channel != NULL) {
        setHVolume(fp, 0);
        pauseH(fp);
        result = FMOD_System_Update(finst->system);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }

        int channelIndex;
        result = FMOD_Channel_GetIndex(fp->channel, &channelIndex);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
        } else {
             // Search for the player in the array and remove it
            for (int i = 0; i < numPlayers; i++) {
                if (allPlayers[i]->path == fp->path) {
                    // Shift all elements after this one to the left
                    for (int j = i; j < numPlayers - 1; j++) {
                        allPlayers[j] = allPlayers[j + 1];
                    }
                    numPlayers--;
                    break;
                }
            }
        }

        result = FMOD_Channel_Stop(fp->channel);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }
        fp->channel = NULL;
    }

    if (fp->sound != NULL) {
        result = FMOD_Sound_Release(fp->sound);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }

        result = FMOD_System_Update(finst->system);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }
        fp->sound = NULL;
    }

    free(fp);
    fp = NULL;
    return (void *)fp;
}

void playH(void *handle) {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];
    FMOD_CHANNELGROUP *mastergroup;
    FMOD_BOOL isPaused;

    fplayer *fp = (fplayer *)handle;

    if (fp == NULL) { // means memory was already cleared
        errorLogger((char*)"ERROR: attempt to play while not initialized");
        return;
    }

    if (fp->channel == NULL) { // when we play sound for the first time after loading
        result = FMOD_System_GetMasterChannelGroup(finst->system, &mastergroup);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return;
        }

        result = FMOD_System_PlaySound(finst->system, fp->sound, NULL, true, &fp->channel);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
        }
        result = FMOD_System_Update(finst->system);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return;
        }

        int channelIndex;
        result = FMOD_Channel_GetIndex(fp->channel, &channelIndex);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
        } else {
            allPlayers[numPlayers++] = fp;
        }
        setHVolume(fp, fp->volume);
    }

    result = FMOD_Channel_GetPaused(fp->channel, &isPaused);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    if (!isPaused) {
        return;
    }

    result = FMOD_Channel_SetPaused(fp->channel, false);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
    result = FMOD_System_Update(finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}

void *createStream(char* path, double volume, bool looped) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    fplayer *fp = (fplayer *)malloc(sizeof(fplayer));
    fp->sound = NULL;
    fp->channel = NULL;
    fp->volume = (float)volume;
    fp->path = path;
    fp->looped = looped;

    result = FMOD_System_CreateStream(finst->system, path, looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, NULL, &fp->sound);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return NULL;
    }

    return (void *)fp;
}

void dispose() {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];

    for (int i = 0; i < numPlayers; i++) {
        stopH(allPlayers[i]);
    }

    numPlayers = 0;

    if (finst == NULL) {
        if (logger != NULL) {
            commonLogger((char*)"WARN: attempt to dispose while not initialized");
        }
        return;
    }

    result = FMOD_System_Close(finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = FMOD_System_Release(finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    free(finst);
    finst = NULL;
}


void mixerSuspend() {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];

    result = FMOD_System_MixerSuspend(finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}

void mixerResume() {
    FMOD_RESULT result;
    char error[ERR_BUF_SIZE];

    result = FMOD_System_MixerResume(finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}
