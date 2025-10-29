#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

class Player
{
public:
    int x;
    int y;

    Player();
    ~Player();

    char *DrawSprite(u_long *ot, char *pri);
};