#include "region.hpp"
#include "graph.hpp"
#include "sound.hpp"
#include "cdrom.hpp"
#include "controller.hpp"
#include "memcard.hpp"
#include "player.hpp"
#include "scrystal.hpp"

typedef struct {
    u_short r;
    u_short g;
    u_short b;
    u_short stp;
}CCLUT;

class GameEngine
{
public:
    GameEngine();
    ~GameEngine();

    TMD_PRIM *mdls;

    CCLUT cclut;
    int cclut_switch;

    GameRegion region;
    GameGraph graph;
    GameSound sound;
    GameCDROM cdrom;
    GameController controller;
    SigmaCrystal scrystal;
    Player player;
    Player player2;
    Player player3;
    Player player4;
    GameSave memcard;

    u_short customClut[16];

    VAGsound snd;

    // bool failed = false;

    void GameInit();
    void GameLoadStuff();
    void GameLoop();
    void GameResetGarbage();
};