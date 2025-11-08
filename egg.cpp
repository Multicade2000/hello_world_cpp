#include "egg.hpp"

Egg::Egg()
{
    x = 0;
    y = 0;

    speed_y = 0;

    RotVector = {0,0,0};
    MovVector = {0,0,256,0};
    ScaleVector = {ONE,ONE,ONE};
    PolyMatrix = {0};

    VertPos[0] = {-8, -12, 1};
    VertPos[1] = {-8, 12, 1};
    VertPos[2] = {8, -12, 1};
    VertPos[3] = {8, 12, 1};

    next = nullptr;
}

Egg::~Egg()
{
    free(next);
}

char *Egg::DrawSprite(u_long *ot, char *pri, int max_ot)
{
    long polydepth;
    long polyflag;

    long OTz;

    MovVector.vx = x-248;
    MovVector.vy = y-224;

    RotMatrix(&RotVector, &PolyMatrix);
    TransMatrix(&PolyMatrix, &MovVector);
    ScaleMatrix(&PolyMatrix, &ScaleVector);
        
    SetRotMatrix(&PolyMatrix);
    SetTransMatrix(&PolyMatrix);

    POLY_FT4 *poly = (POLY_FT4 *)pri;
    setPolyFT4(poly);

    poly->tpage = getTPage(0,0,512,0);
    poly->clut = getClut(512,449);

    setRGB0(poly, 128, 128, 128);
        
    OTz = RotTransPers(&VertPos[0], (long *)&poly->x0, &polydepth, &polyflag);
    OTz += RotTransPers(&VertPos[1], (long *)&poly->x1, &polydepth, &polyflag);
    OTz += RotTransPers(&VertPos[2], (long *)&poly->x2, &polydepth, &polyflag);
    OTz += RotTransPers(&VertPos[3], (long *)&poly->x3, &polydepth, &polyflag);

    OTz /= 4;
        
    setUV4(poly, 0, 64, 0, 64+24, 16, 64, 16, 64+24);

    if ((OTz > 0) && (OTz < max_ot))
        addPrim(ot[OTz-3], poly);
        
    return pri+sizeof(POLY_FT4);

    // SPRT *sprt;
    // DR_TPAGE *tpage;

    // sprt = (SPRT*)pri;
    // setSprt(sprt);

    // setXY0(sprt, x, y);
    // setWH(sprt, 16, 24);
    // setUV0(sprt, 0, 64);
    // setRGB0(sprt,
    //     128,
    //     128,
    //     128);
    // sprt->clut = getClut(512,449);
    
    // addPrim(ot, sprt);
    // pri += sizeof(SPRT);

    // tpage = (DR_TPAGE*)pri;
    // setDrawTPage(tpage, 0, 1, getTPage(0,2,512,0));
    // addPrim(ot, tpage);

    // return pri+sizeof(DR_TPAGE);
}

bool Egg::IsOffScreen(int ResW, int ResH)
{
    return (x < 0 || x > ResW) || (y < 0 || y > ResH);
}

void Egg::Update()
{
    if (ScaleVector.vx > 0)
    {
        x += 2;
    }
    else if (ScaleVector.vx < 0)
    {
        x -= 2;
    }

    y += speed_y;
}