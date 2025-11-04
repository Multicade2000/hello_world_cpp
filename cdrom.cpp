#include "cdrom.hpp"

GameCDROM::GameCDROM()
{
    cd_tick = 0;
    cd_standby = false;
}

GameCDROM::~GameCDROM()
{

}

void GameCDROM::CDROM_Init()
{
    CdInit();
}

u_long* GameCDROM::CDROM_ReadFile(const char* filename)
{
    CdlFILE	file;
    u_long	*buffer;

    cd_standby = false;
    cd_tick = 0;

    if( !CdSearchFile( &file, (char*)filename ) )
    {
        return NULL;
    }
    
    buffer = (u_long*)malloc( 2048*((file.size+2047)/2048) );

    CdControl( CdlSetloc, (u_char*)&file.pos, 0 );

    CdRead( (file.size+2047)/2048, buffer, CdlModeSpeed );

    CdReadSync( 0, 0 );

    return buffer;
}

void GameCDROM::CDROM_Standby()
{
    if (cd_standby && cd_tick != 0)
    {
        u_char param = 0x00;
        CdControl(CdlSetmode,&param,0);
        CdControl(CdlStandby,0,0);
        cd_tick = 0;
    }
    else
    {
        cd_tick++;
        if (cd_tick >= 1000)
        {
            cd_standby = true;
        }
    }
}