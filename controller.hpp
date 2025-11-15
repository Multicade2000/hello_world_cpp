#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libpad.h>

#define PAD_SELECT      1
#define PAD_L3          2
#define PAD_R3          4
#define PAD_START       8
#define PAD_UP          16
#define PAD_RIGHT       32
#define PAD_DOWN        64
#define PAD_LEFT        128
#define PAD_L2          256
#define PAD_R2          512
#define PAD_L1          1024
#define PAD_R1          2048
#define PAD_TRIANGLE    4096
#define PAD_CIRCLE      8192
#define PAD_CROSS       16384
#define PAD_SQUARE      32768

class GameController
{
public:

    u_char padbuff[2][34];
    u_char align[6];
    u_char motor[4][2];
    u_short a, b, pad;

    GameController();
    ~GameController();

    bool cross_pressed[4];
    bool mus_pressed[4];
    bool vib_pressed[4];
    bool save_pressed[4];
    bool xa_pressed[4];

    bool vib_sync[4];

    void ControllerInit();
    int IsConnected(int port);
    u_char CheckType(int port);
    u_short CheckButton(int port);
    u_char CheckStick(int port, int stick);

    void LoopVibrator(int port);
    
    static GameController* instance;
};