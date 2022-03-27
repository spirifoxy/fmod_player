#include <map>

#ifndef NATIVEPLAYER_INCLUDED
#define NATIVEPLAYER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FMOD::Channel *channel;
    FMOD::Sound *sound;
    const char *path;
    float volume;
    bool looped;
} fplayer;

typedef struct {
    FMOD::System *system;
} sfmod;

__attribute__((used))
std::map<int, fplayer*> allPlayers;
sfmod *finst = nullptr;

void (*logger)(char *, bool isError) = nullptr;
void errorLogger(char *str)
{
    return logger(str, true);
}
void commonLogger(char *str)
{
    return logger(str, false);
}

#ifdef __cplusplus
}
#endif

#endif