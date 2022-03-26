#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "fmod.hpp"
#include "common.h"
#include "native_player.hpp"

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void initialize(void (*printCallback)(char *)) {
    if (finst != nullptr) {
        print((char*)"Was already initialized, returning");
        return;
    }

    FMOD_RESULT    result;
    char           error[ERR_BUF_SIZE];

    print = printCallback;
    finst = (sfmod *)malloc(sizeof(sfmod));
    result = FMOD::System_Create(&finst->system);
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }

    result = finst->system->init(16, FMOD_INIT_NORMAL, nullptr);
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }

    print((char*)"INFO: native player Initialized");
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

    result = finst->system->close();
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }

    result = finst->system->release();
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }

    free(finst->system);
    free(finst);
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void setVolume(void *handle, double_t volume) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *) handle;

    fp->volume = (float)volume;
    result = fp->channel->setVolume(fp->volume);
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }

    result = finst->system->update();
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void pause(void *handle) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *)handle;
    if (fp->channel == nullptr) {
        print((char*)"ERROR: pause called while channel is null");
        return;
    }

    result = fp->channel->setPaused(true);
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }

    result = finst->system->update();
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void stop(void *handle) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];

    auto *fp = (fplayer *)handle;
    if (fp->channel != nullptr) {
        setVolume(fp, 0);
        pause(fp);
        result = finst->system->update();
        if (ERRCHECK(result, error)) {
            print(error);
            return;
        }

        int channelIndex;
        result = fp->channel->getIndex(&channelIndex);
        if (ERRCHECK(result, error)) {
            print(error);
        } else {
            if (allPlayers.count(channelIndex)) {
                allPlayers.erase(channelIndex);
            }
        }
    }

    if (fp->sound != nullptr) {
        fp->sound->release();
        result = finst->system->update();
        if (ERRCHECK(result, error)) {
            print(error);
            return;
        }
    }

    free(fp);
}

extern "C" __attribute__((visibility("default"))) __attribute__((used))
void play(void *handle) {
    FMOD_RESULT         result;
    char                error[ERR_BUF_SIZE];
    FMOD::ChannelGroup  *mastergroup;

    auto *fp = (fplayer *)handle;

    if (fp->channel == nullptr) {
        result = finst->system->getMasterChannelGroup(&mastergroup);
        if (ERRCHECK(result, error)) {
            print(error);
            return;
        }

        result = finst->system->playSound(fp->sound, mastergroup, true, &fp->channel);
        if (ERRCHECK(result, error)) {
            print(error);
        }
        result = finst->system->update();
        if (ERRCHECK(result, error)) {
            print(error);
            return;
        }

        int channelIndex;
        result = fp->channel->getIndex(&channelIndex);
        if (ERRCHECK(result, error)) {
            print(error);
        } else {
            allPlayers[channelIndex] = fp;
        }
        setVolume(fp, fp->volume);
    }

    result = fp->channel->setPaused(false);
    if (ERRCHECK(result, error)) {
        print(error);
        return;
    }
    result = finst->system->update();
    if (ERRCHECK(result, error)) {
        print(error);
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

    result = finst->system->createStream(path, looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, nullptr, &fp->sound);
    if (ERRCHECK(result, error)) {
        print(error);
        return nullptr;
    }

    return (void *)fp;
}
