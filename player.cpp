#include "player.hpp"

Player::Player()
{
    x = 48;
    y = 48;
}

Player::~Player()
{

}

char *Player::DrawSprite(u_long *ot, char *pri)
{
    SPRT *sprt;
    DR_TPAGE *tpage;

    sprt = (SPRT*)pri;
    setSprt(sprt);

    setXY0(sprt, x, y);
    setWH(sprt, 64, 64);
    setUV0(sprt, 0, 0);
    setRGB0(sprt,
        128,
        128,
        128);
    sprt->clut = getClut(512,448);
    
    addPrim(ot, sprt);
    pri += sizeof(SPRT);

    tpage = (DR_TPAGE*)pri;
    setDrawTPage(tpage, 0, 1, getTPage(0,2,512,0));
    addPrim(ot, tpage);

    return pri+sizeof(DR_TPAGE);
}