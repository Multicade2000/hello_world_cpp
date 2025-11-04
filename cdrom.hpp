#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libspu.h>
#include <libcd.h>
#include <malloc.h>

#define CD_SECTOR_SIZE 2048

class GameCDROM
{
public:
    GameCDROM();
    ~GameCDROM();

    int cd_tick;
    bool cd_standby;

    int pos_start;
    int pos_end;
    int cur_pos;

    bool doubleSpeed;

    bool xa_playing;

    void CDROM_Init();

    void CDROM_XAPlay(const char *XAload, u_char channel, bool dS);
    void CDROM_XAUpdate();
    void CDROM_XAStop();

    u_long *CDROM_ReadFile(const char* filename);

    void CDROM_Standby();
};