#include "region.hpp"
#include "graph.hpp"
#include "sound.hpp"
#include "cdrom.hpp"
#include "controller.hpp"
#include "memcard.hpp"
#include "player.hpp"

class GameEngine
{
public:
    GameEngine();
    ~GameEngine();

    GameRegion region;
    GameGraph graph;
    GameSound sound;
    GameCDROM cdrom;
    GameController controller;
    Player player;
    Player player2;
    GameSave memcard;

    VAGsound snd;

    // bool failed = false;

    void GameInit();
    void GameLoadStuff();
    void GameLoop();
};