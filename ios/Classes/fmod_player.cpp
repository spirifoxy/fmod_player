#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "fmod.hpp"
#include "common.h"
#include "native_player.hpp"

extern "C" __attribute__((visibility("default"))) __attribute__((used))
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

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void initialize(void (*printCallback)(char *, bool)) {
    if (logger == nullptr) {
        logger = printCallback;
    }

    if (finst != nullptr) {
        commonLogger((char*)"WARN: was already initialized, returning");
        return;
    }

    FMOD_RESULT    result;
    char           error[ERR_BUF_SIZE];

    finst = (sfmod *)malloc(sizeof(sfmod));
    result = FMOD::System_Create(&finst->system);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = finst->system->init(16, FMOD_INIT_NORMAL, nullptr);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = finst->system->setFileSystem(0, 0, myread, 0, 0, 0, 2048);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    commonLogger((char*)"INFO: native player Initialized");
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void dispose() {
    FMOD_RESULT    result;
    char           error[ERR_BUF_SIZE];

    std::map<int, fplayer*>::iterator it;
    for (it = allPlayers.begin(); it != allPlayers.end(); it++) {
        fplayer *player = it->second;
        free(player);
    }

    if (finst == nullptr) {
        if (logger != nullptr) {
            commonLogger((char*)"WARN: attempt to dispose while not initialized");
        }
        return;
    }

    result = finst->system->close();
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = finst->system->release();
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    free(finst);
    finst = nullptr;
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void setVolume(void *handle, double_t volume) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *) handle;

    if (fp == nullptr) {
        errorLogger((char*)"ERROR: attempt to change volume while not initialized");
        return;
    }

    fp->volume = (float)volume;
    result = fp->channel->setVolume(fp->volume);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = finst->system->update();
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void pause(void *handle) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *)handle;

    if (fp == nullptr) { // means memory was already cleared
        errorLogger((char*)"ERROR: attempt to pause while not initialized");
        return;
    }

    if (fp->channel == nullptr) {
        errorLogger((char*)"ERROR: pause called while channel is null");
        return;
    }

    result = fp->channel->setPaused(true);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    result = finst->system->update();
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void *stop(void *handle) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *)handle;

    if (fp == nullptr) { // means memory was already cleared
        errorLogger((char*)"ERROR: attempt to stop while not initialized");
        return nullptr;
    }

    if (fp->channel != nullptr) {
        setVolume(fp, 0);
        pause(fp);
        result = finst->system->update();
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }

        int channelIndex;
        result = fp->channel->getIndex(&channelIndex);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
        } else {
            if (allPlayers.count(channelIndex)) {
                allPlayers.erase(channelIndex);
            }
        }

        result = fp->channel->stop();
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }
        fp->channel = nullptr;
    }

    if (fp->sound != nullptr) {
        fp->sound->release();
        result = finst->system->update();
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return (void *)fp;
        }
        fp->sound = nullptr;
    }

    free(fp);
    fp = nullptr;
    return (void *)fp;
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void play(void *handle) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];
    FMOD::ChannelGroup  *mastergroup;
    bool                isPaused;

    auto *fp = (fplayer *)handle;

    if (fp == nullptr) { // means memory was already cleared
        errorLogger((char*)"ERROR: attempt to play while not initialized");
        return;
    }

    if (fp->channel == nullptr) { // when we play sound for the first time after loading
        result = finst->system->getMasterChannelGroup(&mastergroup);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return;
        }

        result = finst->system->playSound(fp->sound, mastergroup, true, &fp->channel);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
        }
        result = finst->system->update();
        if (ERRCHECK(result, error)) {
            errorLogger(error);
            return;
        }

        int channelIndex;
        result = fp->channel->getIndex(&channelIndex);
        if (ERRCHECK(result, error)) {
            errorLogger(error);
        } else {
            allPlayers[channelIndex] = fp;
        }
        setVolume(fp, fp->volume);
    }

    result = fp->channel->getPaused(&isPaused);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }

    if (!isPaused) {
        return;
    }

    result = fp->channel->setPaused(false);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
    result = finst->system->update();
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return;
    }
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void *createStream(const char* path, double_t volume, bool looped = false) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *)malloc(sizeof(fplayer));
    fp->sound = nullptr;
    fp->channel = nullptr;
    fp->volume = (float)volume;
    fp->path = path;
    fp->looped = looped;

    result = finst->system->createStream(path, looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, nullptr, &fp->sound);
    if (ERRCHECK(result, error)) {
        errorLogger(error);
        return nullptr;
    }

    return (void *)fp;
}
