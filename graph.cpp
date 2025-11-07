#include "graph.hpp"

GameGraph::GameGraph()
{
    ResW = 512;
    ResH = 480;
    pal_mode = false;
    db = 0;
}

GameGraph::~GameGraph()
{

}

void GameGraph::GraphInit()
{
    ResetGraph(0);
    SetDispMask(0);

    InitGeom();
    SetGeomOffset(ResW / 2, ResH / 2);
    SetGeomScreen(ResW / 2);

    ClearVRAM();

    if (pal_mode)
    {
        ResH = ResH > 256 ? 512 : 256;
    }

    SetDefDispEnv(&disp[0], 0, 0, ResW, ResH);
    SetDefDispEnv(&disp[1], 0, (ResH > 256 ? 0 : ResH), ResW, ResH);
    SetDefDrawEnv(&draw[0], 0, 0, ResW, ResH);
    SetDefDrawEnv(&draw[1], 0, (ResH > 256 ? 0 : ResH), ResW, ResH);

    if (pal_mode)
    {
        disp[0].screen.y = 24;
        disp[1].screen.y = 24;
    }

    if (ResH > 256)
    {
        disp[0].isinter = 1;
        disp[1].isinter = 1;
    }

    SetVideoMode(pal_mode ? MODE_PAL : MODE_NTSC);

    setRGB0(&draw[0], 63, 63, 63);
    draw[0].isbg = 1;
    setRGB0(&draw[1], 63, 63, 63);
    draw[1].isbg = 1;

    nextpri = pribuff[0];

    // FntLoad(960, 256);
    // FntOpen(32, 48, ResW - 32, ResH - 48, 0, 400);
}

void GameGraph::LoadTexture(u_long *tim)
{
    TIM_IMAGE *tparam;

    OpenTIM(tim);
    ReadTIM(tparam);

    LoadImage(tparam->prect, (u_long *)tparam->paddr);
    DrawSync(0);

    if (tparam->mode & 0x8)
    {
        LoadImage(tparam->crect, (u_long *)tparam->caddr);
        DrawSync(0);
    }
}

void GameGraph::GraphDisp()
{
    DrawSync(0);
    VSync(0);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    SetDispMask(1);

    DrawOTag(ot[db] + OTLEN - 1);

    db = !db;
    nextpri = pribuff[db];
}

void GameGraph::CleanOT()
{
    ClearOTagR(ot[db], OTLEN);
}

void GameGraph::ClearVRAM()
{
    for (int x = 0; x < 1024; x += 64)
    {
        for (int y = 0; y < 512; y += 64)
        {
            RECT rect;
            setRECT(&rect, x, y, 64, 64);
            ClearImage2(&rect, 0, 0, 0);
        }
    }
}

void GameGraph::DrawBack()
{
    for (int x = 0; x < ResW; x += 256)
    {
        for (int y = 0; y < ResH; y += 256)
        {
            int w = 256;
            int h = 256;

            if (y == 256 && !pal_mode)
            {
                h -= 32;
            }

            SPRT *sprt;
            DR_TPAGE *tpage;

            sprt = (SPRT *)nextpri;
            setSprt(sprt);

            setXY0(sprt, x, y);
            setWH(sprt, w, h);
            setUV0(sprt, 0, 0);
            setRGB0(sprt,
                    127,
                    127,
                    127);
            sprt->clut = getClut(512,451);

            addPrim(ot[db][OTLEN - 1], sprt);
            nextpri += sizeof(SPRT);

            tpage = (DR_TPAGE *)nextpri;
            setDrawTPage(tpage, 0, 1, getTPage(0, 0, 640+(x/4), y));
            addPrim(ot[db][OTLEN - 1], tpage);

            nextpri += sizeof(DR_TPAGE);
        }
    }
}