#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define OTLEN 512

class GameGraph
{
public:
    GameGraph();
    ~GameGraph();

    DISPENV disp[2];
    DRAWENV draw[2];
    int db;

    u_long ot[2][OTLEN];
    char pribuff[2][32768];
    char *nextpri;

    int ResW;
    int ResH;
    bool pal_mode;

    void GraphInit();
    void LoadTexture(u_long *tim);
    void GraphDisp();

    void CleanOT();

    void ClearVRAM();
};