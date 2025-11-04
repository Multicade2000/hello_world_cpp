#include "cdrom.hpp"

GameCDROM::GameCDROM()
{
    cd_tick = 0;
    cd_standby = false;
    xa_playing = false;
    pos_start = 0;
    pos_end = 0;
    cur_pos = 0;
    doubleSpeed = false;
}

GameCDROM::~GameCDROM()
{

}

void GameCDROM::CDROM_Init()
{
    CdInit();
}

void GameCDROM::CDROM_XAPlay(const char *loadXA, u_char channel, bool dS)
{
    if (!xa_playing)
    {
        cd_standby = false;
        cd_tick = 0;

        CdlFILE XAPos;
        if (!CdSearchFile(&XAPos, (char *)loadXA))
        {
            return;
        }
        pos_start = CdPosToInt(&XAPos.pos);
        pos_end = pos_start + ((XAPos.size / CD_SECTOR_SIZE)*3) - 100;

        u_char param = CdlModeRT | CdlModeSF | CdlModeSize1;
        if (dS)
        {
            param |= CdlModeSpeed;
        }

        doubleSpeed = dS;

        CdControlB(CdlSetmode, &param, 0);
        CdControlF(CdlPause, 0);

        CdlFILTER filter;

        filter.file = 1;
        filter.chan = channel;

        CdControlF(CdlSetfilter, (u_char *)&filter);

        CdlLOC loc;
        cur_pos = pos_start;

        CdIntToPos(pos_start, &loc);
        CdControlF(CdlReadS, (u_char *)&loc);

        xa_playing = true;
    }
}

void GameCDROM::CDROM_XAUpdate()
{
    if (xa_playing)
    {
        cur_pos += doubleSpeed ? 8 : 4;
        if (cur_pos >= pos_end)
        {
            CDROM_XAStop();
        }
    }
}

void GameCDROM::CDROM_XAStop()
{
    if (xa_playing)
    {
        u_char param = doubleSpeed ? CdlModeSpeed : 0x00;

        CdControlF(CdlPause, 0);
        CdControlB(CdlSetmode, &param, 0);
        xa_playing = false;
        pos_start = 0;
        pos_end = 0;
        cur_pos = 0;
    }
}

u_long *GameCDROM::CDROM_ReadFile(const char *filename)
{
    CDROM_XAStop();

    CdlFILE file;
    u_long *buffer;

    cd_standby = false;
    cd_tick = 0;

    if (!CdSearchFile(&file, (char *)filename))
    {
        return NULL;
    }

    buffer = (u_long *)malloc(2048 * ((file.size + 2047) / 2048));

    CdControl(CdlSetloc, (u_char *)&file.pos, 0);

    CdRead((file.size + 2047) / 2048, buffer, CdlModeSpeed);

    CdReadSync(0, 0);

    u_char param = CdlModeSpeed;

    CdControlB(CdlSetmode,&param,0);
    CdControlF(CdlPause, 0);

    return buffer;
}

void GameCDROM::CDROM_Standby()
{
    if (cd_standby)
    {
        if (cd_tick != 0)
        {
            u_char param = 0x00;
            CdControlB(CdlSetmode, &param, 0);
            CdControlF(CdlStandby, 0);
            cd_tick = 0;
        }
    }
    else if (!xa_playing)
    {
        cd_tick++;
        if (cd_tick >= 1000)
        {
            cd_standby = true;
        }
    }
}