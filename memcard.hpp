#include <sys/file.h>
#include <sys/types.h>
#include <kernel.h>
#include <stdio.h>
#define _WCHAR_T
#include <stdlib.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libmcrd.h>

#define SAVENAME_EU "BEMAIN-EXE00HELOCPP0"
#define SAVENAME_US "BAMAIN-EXE00HELOCPP0"
#define SAVENAME_JP "BIMAIN-EXE00HELOCPP0"

typedef struct {
    char id[2];
    char type;
    char size;
    char title[64];
    char pad[28];
    char clut[32];
} SAVEHDR;

typedef struct {
    char sprt[128];
} SAVEIMG;

typedef struct {
    int data[32];
} SAVEDATA;

class GameSave
{
public:
    GameSave();
    ~GameSave();

    long cardCmd;
    long slotResult[1];
    const char *save_name;

    SAVEHDR mem_header;

    SAVEIMG icon;

    void MemCard_Init();
    void PrepareHeader(u_long *icon);
    void MemCard_Save(u_char port, int plr_x, int plr_y, int region);
    SAVEDATA MemCard_Load(u_char port, int region);
};