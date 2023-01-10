#ifndef NATIVEPLAYER_INCLUDED
#define NATIVEPLAYER_INCLUDED

#include <stdio.h>
#include <fmod.h>

#define bool int
#define true 1
#define false 0

#define MAX_PLAYERS 128

typedef struct {
    FMOD_CHANNEL *channel;
    FMOD_SOUND *sound;
    char *path;
    float volume;
    bool looped;
} fplayer;

typedef struct {
    FMOD_SYSTEM *system;
} sfmod;

fplayer *allPlayers[MAX_PLAYERS];
int numPlayers = 0;

sfmod *finst = NULL;

void (*logger)(char *, bool isError) = NULL;

void errorLogger(char *str)
{
    return logger(str, true);
}

void commonLogger(char *str)
{
    return logger(str, false);
}

#endif