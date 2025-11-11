#include "memcard.hpp"

GameSave::GameSave()
{
}

GameSave::~GameSave()
{
}

void GameSave::MemCard_Init()
{
    MemCardInit(0);
    MemCardStart();
}

void GameSave::ClearHeader()
{
    free(&icon);
    free(&mem_header);
}

void GameSave::PrepareHeader(u_long *icon_data, int region)
{
    TIM_IMAGE *tim;

    mem_header.id[0] = 'S';
    mem_header.id[1] = 'C';

    mem_header.type = 0x11;
    mem_header.size = 1;

    if (region == 0)
    {
        mem_header.title[0] = 'S';
        mem_header.title[1] = 'A';
        mem_header.title[2] = 'L';
        mem_header.title[3] = 'U';
        mem_header.title[4] = 'T';
        mem_header.title[5] = 'C';
        mem_header.title[6] = '+';
        mem_header.title[7] = '+';
    }
    else if (region == 1)
    {
        mem_header.title[0] = 'H';
        mem_header.title[1] = 'E';
        mem_header.title[2] = 'L';
        mem_header.title[3] = 'O';
        mem_header.title[4] = 'C';
        mem_header.title[5] = 'P';
        mem_header.title[6] = 'P';
    }
    else if (region == 2)
    {
        mem_header.title[0] = 0x83;
        mem_header.title[1] = 0x6E;
        mem_header.title[2] = 0x83;
        mem_header.title[3] = 0x8D;
        mem_header.title[4] = 0x81;
        mem_header.title[5] = 0x5B;
        mem_header.title[6] = 0x82;
        mem_header.title[7] = 0x62;
        mem_header.title[8] = 0x81;
        mem_header.title[9] = 0x7B;
        mem_header.title[10] = 0x81;
        mem_header.title[11] = 0x7B;
    }

    OpenTIM(icon_data);
    ReadTIM(tim);

    __builtin_memcpy(mem_header.clut, tim->caddr, 32);
    __builtin_memcpy(icon.sprt, tim->paddr, 128);
}

void GameSave::MemCard_Save(u_char port, int plr_x, int plr_y, int region)
{
    const char *save_name = "__MAIN-EXE00HELOCPP0";

    if (region == 0)
    {
        __builtin_strcpy((char *)save_name, SAVENAME_EU);
    }
    else if (region == 1)
    {
        __builtin_strcpy((char *)save_name, SAVENAME_US);
    }
    else if (region == 2)
    {
        __builtin_strcpy((char *)save_name, SAVENAME_JP);
    }

    SAVEDATA dump_data;

    dump_data.data[0] = plr_x;
    dump_data.data[1] = plr_y;

    MemCardExist(port);
    MemCardSync(0, &cardCmd, &slotResult[0]);

    if ((slotResult[0] == McErrNone) || (slotResult[0] == McErrNewCard))
    {
        MemCardAccept(port);
        MemCardSync(0, &cardCmd, &slotResult[0]);
    }
    else if ((slotResult[0] == McErrNotFormat))
    {
        MemCardFormat(port);
    }
    else
    {
        return;
    }

    if (MemCardOpen(port, (char *)save_name, O_WRONLY) == McErrFileNotExist)
    {

        if (MemCardCreateFile(port, (char *)save_name, 1) == 0x07)
        {
            return;
        }
        MemCardOpen(port, (char *)save_name, O_WRONLY);
    }

    MemCardWriteData((u_long *)&mem_header, 128 * 0, 128);
    MemCardSync(0, &cardCmd, &slotResult[0]);

    MemCardWriteData((u_long *)&icon, 128 * 1, 128);
    MemCardSync(0, &cardCmd, &slotResult[0]);

    MemCardWriteData((u_long *)&dump_data, 128 * 2, 128);
    MemCardSync(0, &cardCmd, &slotResult[0]);

    MemCardClose();
}

SAVEDATA GameSave::MemCard_Load(u_char port, int region)
{
    const char *save_name = "__MAIN-EXE00HELOCPP0";

    if (region == 0)
    {
        __builtin_strcpy((char *)save_name, SAVENAME_EU);
    }
    else if (region == 1)
    {
        __builtin_strcpy((char *)save_name, SAVENAME_US);
    }
    else if (region == 2)
    {
        __builtin_strcpy((char *)save_name, SAVENAME_JP);
    }

    SAVEDATA dump_data;
    dump_data.data[0] = 48;
    dump_data.data[1] = 48;

    MemCardExist(port);
    MemCardSync(0, &cardCmd, &slotResult[0]);

    if ((slotResult[0] == McErrNone) || (slotResult[0] == McErrNewCard))
    {
        MemCardAccept(port);
        MemCardSync(0, &cardCmd, &slotResult[0]);
    }
    else
    {
        return dump_data;
    }

    if (MemCardOpen(port, (char *)save_name, O_RDONLY) == McErrFileNotExist)
    {
        return dump_data;
    }

    MemCardReadData((u_long *)&dump_data, 128 * 2, 128);
    MemCardSync(0, &cardCmd, &slotResult[0]);

    MemCardClose();

    return dump_data;
}