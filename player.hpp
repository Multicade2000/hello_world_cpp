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

    char pidx;

    Player();
    ~Player();

    SVECTOR RotVector;
    VECTOR  MovVector;
    VECTOR  ScaleVector;
    
    SVECTOR VertPos[4];
    MATRIX PolyMatrix;

    Egg *eggs;

    char *DrawSprite(u_long *ot, char *pri, int max_ot);
    void SpawnEgg(int speed_y);
    char *DrawEggs(u_long *ot, char *pri, int max_ot, int ResW, int ResH);
};