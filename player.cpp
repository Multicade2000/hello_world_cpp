#include "player.hpp"
#include "controller.hpp"

Player::Player()
{
    x = 48;
    y = 48;
    eggs = nullptr;
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

void Player::SpawnEgg()
{
    Egg *mem = (Egg *)malloc(sizeof(Egg));
    if (!mem) return;
    mem->x = x+64;
    mem->y = y+12;

    mem->next = eggs;
    eggs = mem;
}

char *Player::DrawEggs(u_long *ot, char *pri, int ResW, int ResH)
{
    Egg* curr = eggs;
    Egg* prev = nullptr;

    while(curr)
    {
        pri = curr->DrawSprite(ot, pri);
        curr->Update();

        if (curr->IsOffScreen(ResW, ResH))
        {
            Egg* toDelete = curr;
            if (prev) prev->next = curr->next;
            else eggs = curr->next;

            curr = curr->next;

            toDelete->~Egg();
            free(toDelete);
            continue;
        }

        prev = curr;
        curr = curr->next;
    }

    return pri;
}