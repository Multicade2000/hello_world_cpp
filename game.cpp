#include "game.hpp"

GameEngine::GameEngine()
{
    region = GameRegion();
    graph = GameGraph();
    cdrom = GameCDROM();
    sound = GameSound();
    controller = GameController();
    player = Player();
    memcard = GameSave();
}

GameEngine::~GameEngine()
{
}

void GameEngine::GameInit()
{
    graph.pal_mode = region.REGION_CODE == 0 ? true : false;
    graph.GraphInit();
    sound.SoundInit();
    cdrom.CDROM_Init();
    memcard.MemCard_Init();
    controller.ControllerInit();
}

void GameEngine::GameLoadStuff()
{
    u_long *file[3];

    if ((file[0] = cdrom.CDROM_ReadFile("\\DATA\\SPRT\\BIRD.TIM;1")))
    {
        graph.LoadTexture(file[0]);

        free(file[0]);
    }
    // else
    // {
    //     failed = true;
    // }

    if ((file[1] = cdrom.CDROM_ReadFile("\\DATA\\SND\\JUMP.VAG;1")))
    {
        snd.VAGfile = (u_char *)file[1];
        snd.spu_channel = SPU_0CH;
        snd.spu_address = sound.SetSPUtransfer(&snd);

        free(file[1]);
    }

    if ((file[2] = cdrom.CDROM_ReadFile("\\MAIN.TIM;1")))
    {
        memcard.PrepareHeader(file[2]);
        free(file[2]);
    }

    cdrom.CDROM_Standby();
}

void GameEngine::GameLoop()
{
    // if (failed)
    // {
    //     FntPrint("Sprite failed to load!");
    //     FntFlush(-1);
    // }

    graph.CleanOT();
    graph.nextpri += *player.DrawSprite(graph.ot[graph.db], graph.nextpri);

    u_short btn, btn_hold;

    if (controller.IsConnected(0x00))
    {
        if (controller.CheckType(0x00) == 0x4)
        {
            btn = controller.CheckHold(0x00, 1);
            btn_hold = controller.CheckHold(0x00, 0);

            if (btn & PAD_R1)
            {
                controller.StartVibrator(0x00, 1, 0);
            }
            if (btn & PAD_R2)
            {
                controller.StartVibrator(0x00, 0, 0);
            }
            if (btn & PAD_L1)
            {
                controller.StartVibrator(0x00, 0, 1);
            }
            if (btn & PAD_L2)
            {
                controller.StartVibrator(0x00, 0, 0);
            }

            if (btn & PAD_CROSS)
            {
                sound.PlaySFX(&snd);
            }

            if (btn & PAD_START)
            {
                SAVEDATA data = memcard.MemCard_Load();
                player.x = data.data[0];
                player.y = data.data[1];
            }

            if (btn & PAD_SELECT) //The real culprit behind the auto-save bug!
            {
                if (player.x != 48 && player.y != 48)
                {
                    memcard.MemCard_Save(player.x, player.y);
                }
            }

            if (btn_hold & PAD_UP)
            {
                player.y -= 4;
            }
            else if (btn_hold & PAD_DOWN)
            {
                player.y += 4;
            }
            if (btn_hold & PAD_LEFT)
            {
                player.x -= 4;
            }
            else if (btn_hold & PAD_RIGHT)
            {
                player.x += 4;
            }
        }
        else if (controller.CheckType(0x00) == 0x7)
        {
            btn = controller.CheckHold(0x00, 1);
            btn_hold = controller.CheckHold(0x00, 0);
            int rs_x = (int)controller.CheckStick(0x00, 0) - 127;
            int rs_y = (int)controller.CheckStick(0x00, 1) - 127;
            int ls_x = (int)controller.CheckStick(0x00, 2) - 127;
            int ls_y = (int)controller.CheckStick(0x00, 3) - 127;

            if (btn & PAD_R1)
            {
                controller.StartVibrator(0x00, 1, 0);
            }
            if (btn & PAD_R2)
            {
                controller.StartVibrator(0x00, 0, 0);
            }
            if (btn & PAD_L1)
            {
                controller.StartVibrator(0x00, 0, 255);
            }
            if (btn & PAD_L2)
            {
                controller.StartVibrator(0x00, 0, 0);
            }

            if (btn & PAD_CROSS)
            {
                sound.PlaySFX(&snd);
            }

            if (btn & PAD_START)
            {
                SAVEDATA data = memcard.MemCard_Load();
                player.x = data.data[0];
                player.y = data.data[1];
            }

            if (btn & PAD_SELECT) //The real culprit behind the auto-save bug!
            {
                if (player.x != 48 && player.y != 48)
                {
                    memcard.MemCard_Save(player.x, player.y);
                }
            }

            if (ls_y < -15)
            {
                player.y -= -(ls_y / 15);
            }
            if (ls_y > 15)
            {
                player.y += ls_y / 15;
            }
            if (ls_x < -15)
            {
                player.x -= -(ls_x / 15);
            }
            if (ls_x > 15)
            {
                player.x += ls_x / 15;
            }

            if ((btn_hold & PAD_UP) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.y -= 4;
            }
            else if ((btn_hold & PAD_DOWN) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.y += 4;
            }
            if ((btn_hold & PAD_LEFT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.x -= 4;
            }
            else if ((btn_hold & PAD_RIGHT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.x += 4;
            }
        }
    }

    if (player.x < 0)
    {
        player.x = 0;
    }
    else if (player.x > graph.ResW - 64)
    {
        player.x = graph.ResW - 64;
    }

    if (player.y < 0)
    {
        player.y = 0;
    }
    else if (player.y > graph.ResH - 64)
    {
        player.y = graph.ResH - 64;
    }

    graph.GraphDisp();
}