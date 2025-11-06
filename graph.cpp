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
    SetGeomOffset(ResW/2, ResH/2);
    SetGeomScreen(ResW/2);

    ClearVRAM();

    if (pal_mode)
    {
        ResH = ResH > 256 ? 512 : 256;
    }

    SetDispMask(0);

    SetDefDispEnv(&disp[0],0,0,ResW,ResH);
    SetDefDispEnv(&disp[1],0,(ResH > 256 ? 0 : ResH),ResW,ResH);
    SetDefDrawEnv(&draw[0],0,0,ResW,ResH);
    SetDefDrawEnv(&draw[1],0,(ResH > 256 ? 0 : ResH),ResW,ResH);

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

    setRGB0(&draw[0],63,63,63);
    draw[0].isbg = 1;
    setRGB0(&draw[1],63,63,63);
    draw[1].isbg = 1;

    nextpri = pribuff[0];

    FntLoad(960,0);
    FntOpen(32,48,ResW-32,ResH-48,0,400);
}

void GameGraph::LoadTexture(u_long *tim)
{
    TIM_IMAGE *tparam;

    OpenTIM(tim);
    ReadTIM(tparam);

    LoadImage(tparam->prect, (u_long*)tparam->paddr);
    DrawSync(0);

    if( tparam->mode & 0x8 ) {

        LoadImage(tparam->crect, (u_long*)tparam->caddr);
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

    DrawOTag(ot[db]+OTLEN-1);
    
    db = !db;
    nextpri = pribuff[db];
}

void GameGraph::CleanOT()
{
    ClearOTagR(ot[db],OTLEN);
}

void GameGraph::ClearVRAM()
{
    RECT rect;
    setRECT(&rect,0,0,1024,512);
    ClearImage(&rect,0,0,0);
}