#include <sys/types.h>
#include <stdio.h>
#define _WCHAR_T
#include <stdlib.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

class Egg
{
public:
    int x;
    int y;

    int speed_y;

    Egg* next;

    Egg();
    ~Egg();

    SVECTOR RotVector;
    VECTOR  MovVector;
    VECTOR  ScaleVector;
    
    SVECTOR VertPos[4];                                         
    MATRIX PolyMatrix;

    char *DrawSprite(u_long *ot, char *pri, int max_ot);
    bool IsOffScreen(int ResW, int ResH);
    void Update();
};