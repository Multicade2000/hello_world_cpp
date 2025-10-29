#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libcd.h>
#include <malloc.h>

class GameCDROM
{
public:
    GameCDROM();
    ~GameCDROM();

    void CDROM_Init();
    u_long *CDROM_ReadFile(const char* filename);

    void CDROM_Standby();
};