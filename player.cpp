#include "player.hpp"
#include "controller.hpp"

Player::Player()
{
    x = 48;
    y = 48;

    p2 = false;

    RotVector = {0,0,0};
    MovVector = {0,0,256,0};
    ScaleVector = {ONE,ONE,ONE};
    PolyMatrix = {0};

    VertPos[0] = {-32, -32, 1};
    VertPos[1] = {-32, 32, 1};
    VertPos[2] = {32, -32, 1};
    VertPos[3] = {32, 32, 1};
    eggs = nullptr;
}

Player::~Player()
{
    Egg* curr = eggs;
    Egg* prev = nullptr;

    while(curr)
    {
        Egg* toDelete = curr;
        if (prev) prev->next = curr->next;
        else eggs = curr->next;
        
        curr = curr->next;
        
        toDelete->~Egg();
        free(toDelete);
        continue;

        prev = curr;
        curr = curr->next;
    }
    
    free(eggs);

    x = 48;
    y = 48;
    eggs = nullptr;
}

char *Player::DrawSprite(u_long *ot, char *pri, int max_ot)
{
    // SPRT *sprt;
    // DR_TPAGE *tpage;

    long polydepth;
    long polyflag;

    MovVector.vx = x-224;
    MovVector.vy = y-208;

    long OTz;

    RotMatrix(&RotVector, &PolyMatrix);
    TransMatrix(&PolyMatrix, &MovVector);
    ScaleMatrix(&PolyMatrix, &ScaleVector);
        
    SetRotMatrix(&PolyMatrix);
    SetTransMatrix(&PolyMatrix);
    
    POLY_FT4 *poly = (POLY_FT4 *)pri;
    setPolyFT4(poly);

    poly->tpage = getTPage(0,0,512,0);

    poly->clut = getClut(512,448);

    setRGB0(poly, 128, 128, 128);
        
    OTz = RotTransPers(&VertPos[0], (long *)&poly->x0, &polydepth, &polyflag);
    OTz += RotTransPers(&VertPos[1], (long *)&poly->x1, &polydepth, &polyflag);
    OTz += RotTransPers(&VertPos[2], (long *)&poly->x2, &polydepth, &polyflag);
    OTz += RotTransPers(&VertPos[3], (long *)&poly->x3, &polydepth, &polyflag);

    OTz /= 4;
    
    if (!p2)
    {
        setUV4(poly, 0, 0, 0, 63, 63, 0, 63, 63);
    }
    else
    {
        setUV4(poly, 63, 0, 63, 63, 127, 0, 127, 63);
    }

    if ((OTz > 0) && (OTz < max_ot))
        addPrim(ot[OTz-3], poly);
        
    return pri+sizeof(POLY_FT4);

    // sprt = (SPRT*)pri;
    // setSprt(sprt);

    // setXY0(sprt, x, y);
    // setWH(sprt, 64, 64);
    // setUV0(sprt, 0, 0);
    // setRGB0(sprt,
    //     127,
    //     127,
    //     127);
    // sprt->clut = getClut(512,448);
    
    // addPrim(ot, sprt);
    // pri += sizeof(SPRT);

    // tpage = (DR_TPAGE*)pri;
    // setDrawTPage(tpage, 0, 1, getTPage(0,0,512,0));
    // addPrim(ot, tpage);

    // return pri+sizeof(DR_TPAGE);
}

void Player::SpawnEgg(int speed_y)
{
    Egg *mem = (Egg *)malloc(sizeof(Egg));
    if (!mem) return;

    mem->RotVector = {0,0,0};
    mem->MovVector = {0,0,MovVector.vz,0};
    mem->ScaleVector = {ScaleVector.vx,ONE,ONE};
    mem->PolyMatrix = {0};

    mem->VertPos[0] = {-8, -12, 1};
    mem->VertPos[1] = {-8, 12, 1};
    mem->VertPos[2] = {8, -12, 1};
    mem->VertPos[3] = {8, 12, 1};

    if (ScaleVector.vx > 0)
    {
        mem->x = x+64;
    }
    else if (ScaleVector.vx < 0)
    {
        mem->x = x-16;
    }

    mem->y = y+12;

    mem->speed_y = speed_y;

    mem->next = eggs;
    eggs = mem;
}

char *Player::DrawEggs(u_long *ot, char *pri, int max_ot, int ResW, int ResH)
{
    Egg* curr = eggs;
    Egg* prev = nullptr;

    while(curr)
    {
        pri = curr->DrawSprite(ot, pri, max_ot);
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