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

    Egg* next;

    Egg();
    ~Egg();

    char *DrawSprite(u_long *ot, char *pri);
    bool IsOffScreen(int ResW, int ResH);
    void Update();
};