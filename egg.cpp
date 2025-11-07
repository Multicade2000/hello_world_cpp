#include "egg.hpp"

Egg::Egg()
{
    x = 0;
    y = 0;

    next = nullptr;
}

Egg::~Egg()
{
    free(next);
}

char *Egg::DrawSprite(u_long *ot, char *pri)
{
    SPRT *sprt;
    DR_TPAGE *tpage;

    sprt = (SPRT*)pri;
    setSprt(sprt);

    setXY0(sprt, x, y);
    setWH(sprt, 16, 24);
    setUV0(sprt, 0, 64);
    setRGB0(sprt,
        128,
        128,
        128);
    sprt->clut = getClut(512,449);
    
    addPrim(ot, sprt);
    pri += sizeof(SPRT);

    tpage = (DR_TPAGE*)pri;
    setDrawTPage(tpage, 0, 1, getTPage(0,2,512,0));
    addPrim(ot, tpage);

    return pri+sizeof(DR_TPAGE);
}

bool Egg::IsOffScreen(int ResW, int ResH)
{
    return (x < 0 || x > ResW) || (y < 0 || y > ResH);
}

void Egg::Update()
{
    x += 2;
}