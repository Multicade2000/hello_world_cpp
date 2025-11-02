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
#include <libpad.h>
#include <libmcrd.h>

#define SAVENAME_EU "BEMAIN-EXE00HELOCPP0"
#define SAVENAME_US "BAMAIN-EXE00HELOCPP0"
#define SAVENAME_JP "BIMAIN-EXE00HELOCPP0"

typedef struct {
    char id[2];        // Always 'SC'
    char type;        // Number of icon frames (0x11 - one frame, 0x12 - two frames, 0x13 - three frames)
    char size;        // Size of save file in blocks
    char title[64];    // Title of save file (encoded in Shift-JIS format)
    char pad[28];    // Unused
    char clut[32];    // Color palette of icon frames (16 RGB5X1 16-bit color entries)
} SAVEHDR;

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

    TIM_IMAGE *icon;

    void MemCard_Init();
    void PrepareHeader(u_long *icon);
    void MemCard_Save(u_char port, int plr_x, int plr_y, int region);
    SAVEDATA MemCard_Load(u_char port, int region);
};