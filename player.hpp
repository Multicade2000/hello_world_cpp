#include <sys/types.h>
#include <stdio.h>
#define _WCHAR_T
#include <stdlib.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "egg.hpp"

class Player
{
public:
    int x;
    int y;
    int port;

    Player();
    ~Player();

    Egg *eggs;

    char *DrawSprite(u_long *ot, char *pri);
    void SpawnEgg();
    char *DrawEggs(u_long *ot, char *pri, int ResW, int ResH);
};