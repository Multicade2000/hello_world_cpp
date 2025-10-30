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

void GameSave::PrepareHeader(u_long *icon_data)
{
    mem_header.id[0] = 'S';
    mem_header.id[1] = 'C';

    mem_header.type = 0x11;
    mem_header.size = 1;

    mem_header.title[0] = 'H';
    mem_header.title[1] = 'E';
    mem_header.title[2] = 'L';
    mem_header.title[3] = 'O';
    mem_header.title[4] = 'C';
    mem_header.title[5] = 'P';
    mem_header.title[6] = 'P';

    OpenTIM(icon_data);
    ReadTIM(icon);

    __builtin_memcpy(mem_header.clut, icon->caddr, 32);
}

void GameSave::MemCard_Save(int plr_x, int plr_y, int region)
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

    MemCardExist(0);
    while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    
    if ((slotResult[0] == McErrNone) || (slotResult[0] == McErrNewCard)) {
        MemCardAccept(0);
        while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    }
    else if ((slotResult[0] == McErrNotFormat)) {
        MemCardFormat(0);
    }
    else
    {
        return;
    }

    if (MemCardOpen(0, (char *)save_name, O_WRONLY) == McErrFileNotExist) {
        
        if (MemCardCreateFile(0, (char *)save_name, 1) == 0x07) {
            return;
        }
        MemCardOpen(0, (char *)save_name, O_WRONLY);
    }

    MemCardWriteData((u_long*)&mem_header, 128*0, 128);
    while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    
    MemCardWriteData((u_long*)icon->paddr, 128*1, 128);
    while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    
    MemCardWriteData((u_long*)&dump_data, 128*2, 128);
    while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    
    MemCardClose();
}

SAVEDATA GameSave::MemCard_Load(int region)
{
    char *save_name = "__MAIN-EXE00HELOCPP0";

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

    MemCardExist(0);
    while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    
    if ((slotResult[0] == McErrNone) || (slotResult[0] == McErrNewCard)) {
        MemCardAccept(0);
        while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    }
    else
    {
        return dump_data;
    }
    
    if (MemCardOpen(0, (char *)save_name, O_RDONLY) == McErrFileNotExist) {
        return dump_data;
    }
    
    MemCardReadData((u_long*)&dump_data, 128*2, 128);
    while (MemCardSync(1, &cardCmd, &slotResult[0]) == 0);
    
    MemCardClose();

    return dump_data;
}