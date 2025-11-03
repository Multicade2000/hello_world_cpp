#include "game.hpp"

GameEngine::GameEngine()
{
    region = GameRegion();
    graph = GameGraph();
    cdrom = GameCDROM();
    sound = GameSound();
    GameSound::instance = &sound;
    controller = GameController();
    player = Player();
    player2 = Player();
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
    u_long *file[4];

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
        snd.spu_address = sound.SetSPUtransfer(&snd);

        free(file[1]);
    }

    if ((file[2] = cdrom.CDROM_ReadFile("\\ICON.TIM;1")))
    {
        memcard.PrepareHeader(file[2]);
        free(file[2]);
    }

    if ((file[3] = cdrom.CDROM_ReadFile("\\DATA\\MUS\\INST\\MUSBOX.VAG;1")))
    {
        sound.mus[0].VAGfile = (u_char *)file[3];
        sound.mus[0].spu_address = sound.SetSPUtransfer(&sound.mus[0]);

        free(file[3]);
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

    // FntPrint("%d",mus_size);
    // FntFlush(-1);

    graph.CleanOT();

    controller.LoopVibrator(0x00);
    controller.LoopVibrator(0x10);

    if (controller.IsConnected(0x00))
    {
        if (controller.CheckType(0x00) == 0x4)
        {
            graph.nextpri = player.DrawSprite(graph.ot[graph.db], graph.nextpri);

            u_short btn = controller.CheckButton(0x00);

            if (!(btn & PAD_UP))
            {
                player.y -= 4;
            }
            else if (!(btn & PAD_DOWN))
            {
                player.y += 4;
            }
            if (!(btn & PAD_LEFT))
            {
                player.x -= 4;
            }
            else if (!(btn & PAD_RIGHT))
            {
                player.x += 4;
            }

            if (!(btn & PAD_R1))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 1;
                    controller.motor[0][1] = 0;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else if (!(btn & PAD_R2))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 0;
                    controller.motor[0][1] = 0;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else if (!(btn & PAD_L1))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 0;
                    controller.motor[0][1] = 255;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else if (!(btn & PAD_L2))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 0;
                    controller.motor[0][1] = 0;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else
            {
                controller.vib_pressed[0] = false;
            }

            if (!(btn & PAD_CROSS))
            {
                if (!controller.cross_pressed[0])
                {
                    sound.PlaySFX(&snd, 0, 0xA1BA);
                    controller.cross_pressed[0] = true;
                }
            }
            else
            {
                controller.cross_pressed[0] = false;
            }

            if (!(btn & PAD_SQUARE))
            {
                if (!controller.mus_pressed[0])
                {
                    sound.StopMusic();
                    u_long *file;
                    if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                    {
                        sound.LoadMusic(file, 14, 2);
                        sound.PlayMusic();
                        free(file);
                    }
                    cdrom.CDROM_Standby();
                    controller.mus_pressed[0] = true;
                }
            }
            else if (!(btn & PAD_CIRCLE))
            {
                if (!controller.mus_pressed[0])
                {
                    sound.StopMusic();
                }
            }
            else
            {
                controller.mus_pressed[0] = false;
            }

            if (!(btn & PAD_SELECT))
            {
                if (!controller.save_pressed[0])
                {
                    memcard.MemCard_Save(0x00, player.x, player.y, region.REGION_CODE);
                    controller.save_pressed[0] = true;
                }
            }
            else if (!(btn & PAD_START))
            {
                if (!controller.save_pressed[0])
                {
                    SAVEDATA data = memcard.MemCard_Load(0x00, region.REGION_CODE);
                    player.x = data.data[0];
                    player.y = data.data[1];
                    controller.save_pressed[0] = true;
                }
            }
            else
            {
                controller.save_pressed[0] = false;
            }
        }
        else if (controller.CheckType(0x00) == 0x7)
        {
            graph.nextpri = player.DrawSprite(graph.ot[graph.db], graph.nextpri);

            u_short btn = controller.CheckButton(0x00);
            int rs_x = (int)controller.CheckStick(0x00, 0) - 127;
            int rs_y = (int)controller.CheckStick(0x00, 1) - 127;
            int ls_x = (int)controller.CheckStick(0x00, 2) - 127;
            int ls_y = (int)controller.CheckStick(0x00, 3) - 127;

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

            if (!(btn & PAD_UP) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.y -= 4;
            }
            else if (!(btn & PAD_DOWN) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.y += 4;
            }
            if (!(btn & PAD_LEFT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.x -= 4;
            }
            else if (!(btn & PAD_RIGHT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player.x += 4;
            }

            if (!(btn & PAD_R1))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 1;
                    controller.motor[0][1] = 0;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else if (!(btn & PAD_R2))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 0;
                    controller.motor[0][1] = 0;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else if (!(btn & PAD_L1))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 0;
                    controller.motor[0][1] = 255;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else if (!(btn & PAD_L2))
            {
                if (!controller.vib_pressed[0])
                {
                    controller.motor[0][0] = 0;
                    controller.motor[0][1] = 0;
                    controller.vib_pressed[0] = true;
                    controller.vib_sync[0] = true;
                }
            }
            else
            {
                controller.vib_pressed[0] = false;
            }

            if (!(btn & PAD_CROSS))
            {
                if (!controller.cross_pressed[0])
                {
                    sound.PlaySFX(&snd, 0, 0xA1BA);
                    controller.cross_pressed[0] = true;
                }
            }
            else
            {
                controller.cross_pressed[0] = false;
            }

            if (!(btn & PAD_SQUARE))
            {
                if (!controller.mus_pressed[0])
                {
                    sound.StopMusic();
                    u_long *file;
                    if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                    {
                        sound.LoadMusic(file, 14, 2);
                        sound.PlayMusic();
                        free(file);
                    }
                    cdrom.CDROM_Standby();
                    controller.mus_pressed[0] = true;
                }
            }
            else if (!(btn & PAD_CIRCLE))
            {
                if (!controller.mus_pressed[0])
                {
                    sound.StopMusic();
                }
            }
            else
            {
                controller.mus_pressed[0] = false;
            }

            if (!(btn & PAD_SELECT))
            {
                if (!controller.save_pressed[0])
                {
                    memcard.MemCard_Save(0x00, player.x, player.y, region.REGION_CODE);
                    controller.save_pressed[0] = true;
                }
            }
            else if (!(btn & PAD_START))
            {
                if (!controller.save_pressed[0])
                {
                    SAVEDATA data = memcard.MemCard_Load(0x00, region.REGION_CODE);
                    player.x = data.data[0];
                    player.y = data.data[1];
                    controller.save_pressed[0] = true;
                }
            }
            else
            {
                controller.save_pressed[0] = false;
            }
        }
        else if (controller.CheckType(0x00) == 0x8)
        {
            if (controller.IsConnected(0x10))
            {
                if (controller.CheckType(0x10) == 0x4)
                {
                    graph.nextpri = player.DrawSprite(graph.ot[graph.db], graph.nextpri);

                    u_short btn = controller.CheckButton(0x10);

                    if (!(btn & PAD_UP))
                    {
                        player.y -= 4;
                    }
                    else if (!(btn & PAD_DOWN))
                    {
                        player.y += 4;
                    }
                    if (!(btn & PAD_LEFT))
                    {
                        player.x -= 4;
                    }
                    else if (!(btn & PAD_RIGHT))
                    {
                        player.x += 4;
                    }

                    if (!(btn & PAD_R1))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 1;
                            controller.motor[0][1] = 0;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else if (!(btn & PAD_R2))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 0;
                            controller.motor[0][1] = 0;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else if (!(btn & PAD_L1))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 0;
                            controller.motor[0][1] = 255;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else if (!(btn & PAD_L2))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 0;
                            controller.motor[0][1] = 0;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else
                    {
                        controller.vib_pressed[0] = false;
                    }

                    if (!(btn & PAD_CROSS))
                    {
                        if (!controller.cross_pressed[0])
                        {
                            sound.PlaySFX(&snd, 0, 0xA1BA);
                            controller.cross_pressed[0] = true;
                        }
                    }
                    else
                    {
                        controller.cross_pressed[0] = false;
                    }

                    if (!(btn & PAD_SQUARE))
                    {
                        if (!controller.mus_pressed[0])
                        {
                            sound.StopMusic();
                            u_long *file;
                            if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                            {
                                sound.LoadMusic(file, 14, 2);
                                sound.PlayMusic();
                                free(file);
                            }
                            cdrom.CDROM_Standby();
                            controller.mus_pressed[0] = true;
                        }
                    }
                    else if (!(btn & PAD_CIRCLE))
                    {
                        if (!controller.mus_pressed[0])
                        {
                            sound.StopMusic();
                        }
                    }
                    else
                    {
                        controller.mus_pressed[0] = false;
                    }

                    if (!(btn & PAD_SELECT))
                    {
                        if (!controller.save_pressed[0])
                        {
                            memcard.MemCard_Save(0x00, player.x, player.y, region.REGION_CODE);
                            controller.save_pressed[0] = true;
                        }
                    }
                    else if (!(btn & PAD_START))
                    {
                        if (!controller.save_pressed[0])
                        {
                            SAVEDATA data = memcard.MemCard_Load(0x00, region.REGION_CODE);
                            player.x = data.data[0];
                            player.y = data.data[1];
                            controller.save_pressed[0] = true;
                        }
                    }
                    else
                    {
                        controller.save_pressed[0] = false;
                    }
                }
                else if (controller.CheckType(0x10) == 0x7)
                {
                    graph.nextpri = player.DrawSprite(graph.ot[graph.db], graph.nextpri);

                    u_short btn = controller.CheckButton(0x10);
                    int rs_x = (int)controller.CheckStick(0x10, 0) - 127;
                    int rs_y = (int)controller.CheckStick(0x10, 1) - 127;
                    int ls_x = (int)controller.CheckStick(0x10, 2) - 127;
                    int ls_y = (int)controller.CheckStick(0x10, 3) - 127;

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

                    if (!(btn & PAD_UP) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player.y -= 4;
                    }
                    else if (!(btn & PAD_DOWN) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player.y += 4;
                    }
                    if (!(btn & PAD_LEFT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player.x -= 4;
                    }
                    else if (!(btn & PAD_RIGHT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player.x += 4;
                    }

                    if (!(btn & PAD_R1))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 1;
                            controller.motor[0][1] = 0;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else if (!(btn & PAD_R2))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 0;
                            controller.motor[0][1] = 0;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else if (!(btn & PAD_L1))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 0;
                            controller.motor[0][1] = 255;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else if (!(btn & PAD_L2))
                    {
                        if (!controller.vib_pressed[0])
                        {
                            controller.motor[0][0] = 0;
                            controller.motor[0][1] = 0;
                            controller.vib_pressed[0] = true;
                            controller.vib_sync[0] = true;
                        }
                    }
                    else
                    {
                        controller.vib_pressed[0] = false;
                    }

                    if (!(btn & PAD_CROSS))
                    {
                        if (!controller.cross_pressed[0])
                        {
                            sound.PlaySFX(&snd, 0, 0xA1BA);
                            controller.cross_pressed[0] = true;
                        }
                    }
                    else
                    {
                        controller.cross_pressed[0] = false;
                    }

                    if (!(btn & PAD_SQUARE))
                    {
                        if (!controller.mus_pressed[0])
                        {
                            sound.StopMusic();
                            u_long *file;
                            if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                            {
                                sound.LoadMusic(file, 14, 2);
                                sound.PlayMusic();
                                free(file);
                            }
                            cdrom.CDROM_Standby();
                            controller.mus_pressed[0] = true;
                        }
                    }
                    else if (!(btn & PAD_CIRCLE))
                    {
                        if (!controller.mus_pressed[0])
                        {
                            sound.StopMusic();
                        }
                    }
                    else
                    {
                        controller.mus_pressed[0] = false;
                    }

                    if (!(btn & PAD_SELECT))
                    {
                        if (!controller.save_pressed[0])
                        {
                            memcard.MemCard_Save(0x00, player.x, player.y, region.REGION_CODE);
                            controller.save_pressed[0] = true;
                        }
                    }
                    else if (!(btn & PAD_START))
                    {
                        if (!controller.save_pressed[0])
                        {
                            SAVEDATA data = memcard.MemCard_Load(0x00, region.REGION_CODE);
                            player.x = data.data[0];
                            player.y = data.data[1];
                            controller.save_pressed[0] = true;
                        }
                    }
                    else
                    {
                        controller.save_pressed[0] = false;
                    }
                }
            }
        }
    }
    if (controller.IsConnected(0x01))
    {
        if (controller.CheckType(0x01) == 0x4)
        {
            graph.nextpri = player2.DrawSprite(graph.ot[graph.db], graph.nextpri);

            u_short btn2 = controller.CheckButton(0x01);

            if (!(btn2 & PAD_UP))
            {
                player2.y -= 4;
            }
            else if (!(btn2 & PAD_DOWN))
            {
                player2.y += 4;
            }
            if (!(btn2 & PAD_LEFT))
            {
                player2.x -= 4;
            }
            else if (!(btn2 & PAD_RIGHT))
            {
                player2.x += 4;
            }

            if (!(btn2 & PAD_R1))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 1;
                    controller.motor[1][1] = 0;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else if (!(btn2 & PAD_R2))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 0;
                    controller.motor[1][1] = 0;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else if (!(btn2 & PAD_L1))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 0;
                    controller.motor[1][1] = 255;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else if (!(btn2 & PAD_L2))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 0;
                    controller.motor[1][1] = 0;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else
            {
                controller.vib_pressed[1] = false;
            }

            if (!(btn2 & PAD_CROSS))
            {
                if (!controller.cross_pressed[1])
                {
                    sound.PlaySFX(&snd, 0, 0xA1BA);
                    controller.cross_pressed[1] = true;
                }
            }
            else
            {
                controller.cross_pressed[1] = false;
            }

            if (!(btn2 & PAD_SQUARE))
            {
                if (!controller.mus_pressed[1])
                {
                    sound.StopMusic();
                    u_long *file;
                    if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                    {
                        sound.LoadMusic(file, 14, 2);
                        sound.PlayMusic();
                        free(file);
                    }
                    cdrom.CDROM_Standby();
                    controller.mus_pressed[1] = true;
                }
            }
            else if (!(btn2 & PAD_CIRCLE))
            {
                if (!controller.mus_pressed[1])
                {
                    sound.StopMusic();
                }
            }
            else
            {
                controller.mus_pressed[1] = false;
            }

            if (!(btn2 & PAD_SELECT))
            {
                if (!controller.save_pressed[1])
                {
                    memcard.MemCard_Save(0x10, player2.x, player2.y, region.REGION_CODE);
                    controller.save_pressed[1] = true;
                }
            }
            else if (!(btn2 & PAD_START))
            {
                if (!controller.save_pressed[1])
                {
                    SAVEDATA data = memcard.MemCard_Load(0x10, region.REGION_CODE);
                    player2.x = data.data[0];
                    player2.y = data.data[1];
                    controller.save_pressed[1] = true;
                }
            }
            else
            {
                controller.save_pressed[1] = false;
            }
        }
        else if (controller.CheckType(0x01) == 0x7)
        {
            graph.nextpri = player2.DrawSprite(graph.ot[graph.db], graph.nextpri);

            u_short btn2 = controller.CheckButton(0x01);
            int rs_x = (int)controller.CheckStick(0x01, 0) - 127;
            int rs_y = (int)controller.CheckStick(0x01, 1) - 127;
            int ls_x = (int)controller.CheckStick(0x01, 2) - 127;
            int ls_y = (int)controller.CheckStick(0x01, 3) - 127;

            if (ls_y < -15)
            {
                player2.y -= -(ls_y / 15);
            }
            if (ls_y > 15)
            {
                player2.y += ls_y / 15;
            }
            if (ls_x < -15)
            {
                player2.x -= -(ls_x / 15);
            }
            if (ls_x > 15)
            {
                player2.x += ls_x / 15;
            }

            if (!(btn2 & PAD_UP) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player2.y -= 4;
            }
            else if (!(btn2 & PAD_DOWN) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player2.y += 4;
            }
            if (!(btn2 & PAD_LEFT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player2.x -= 4;
            }
            else if (!(btn2 & PAD_RIGHT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
            {
                player2.x += 4;
            }

            if (!(btn2 & PAD_R1))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 1;
                    controller.motor[1][1] = 0;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else if (!(btn2 & PAD_R2))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 0;
                    controller.motor[1][1] = 0;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else if (!(btn2 & PAD_L1))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 0;
                    controller.motor[1][1] = 255;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else if (!(btn2 & PAD_L2))
            {
                if (!controller.vib_pressed[1])
                {
                    controller.motor[1][0] = 0;
                    controller.motor[1][1] = 0;
                    controller.vib_pressed[1] = true;
                    controller.vib_sync[1] = true;
                }
            }
            else
            {
                controller.vib_pressed[1] = false;
            }

            if (!(btn2 & PAD_CROSS))
            {
                if (!controller.cross_pressed[1])
                {
                    sound.PlaySFX(&snd, 0, 0xA1BA);
                    controller.cross_pressed[1] = true;
                }
            }
            else
            {
                controller.cross_pressed[1] = false;
            }

            if (!(btn2 & PAD_SQUARE))
            {
                if (!controller.mus_pressed[1])
                {
                    sound.StopMusic();
                    u_long *file;
                    if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                    {
                        sound.LoadMusic(file, 14, 2);
                        sound.PlayMusic();
                        free(file);
                    }
                    cdrom.CDROM_Standby();
                    controller.mus_pressed[1] = true;
                }
            }
            else if (!(btn2 & PAD_CIRCLE))
            {
                if (!controller.mus_pressed[1])
                {
                    sound.StopMusic();
                }
            }
            else
            {
                controller.mus_pressed[1] = false;
            }

            if (!(btn2 & PAD_SELECT))
            {
                if (!controller.save_pressed[1])
                {
                    memcard.MemCard_Save(0x10, player2.x, player2.y, region.REGION_CODE);
                    controller.save_pressed[1] = true;
                }
            }
            else if (!(btn2 & PAD_START))
            {
                if (!controller.save_pressed[1])
                {
                    SAVEDATA data = memcard.MemCard_Load(0x10, region.REGION_CODE);
                    player2.x = data.data[0];
                    player2.y = data.data[1];
                    controller.save_pressed[1] = true;
                }
            }
            else
            {
                controller.save_pressed[1] = false;
            }
        }
        else if (controller.CheckType(0x01) == 0x8)
        {
            if (controller.IsConnected(0x20))
            {
                if (controller.CheckType(0x20) == 0x4)
                {
                    graph.nextpri = player2.DrawSprite(graph.ot[graph.db], graph.nextpri);

                    u_short btn2 = controller.CheckButton(0x20);

                    if (!(btn2 & PAD_UP))
                    {
                        player2.y -= 4;
                    }
                    else if (!(btn2 & PAD_DOWN))
                    {
                        player2.y += 4;
                    }
                    if (!(btn2 & PAD_LEFT))
                    {
                        player2.x -= 4;
                    }
                    else if (!(btn2 & PAD_RIGHT))
                    {
                        player2.x += 4;
                    }

                    if (!(btn2 & PAD_R1))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 1;
                            controller.motor[1][1] = 0;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_R2))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 0;
                            controller.motor[1][1] = 0;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_L1))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 0;
                            controller.motor[1][1] = 255;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_L2))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 0;
                            controller.motor[1][1] = 0;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else
                    {
                        controller.vib_pressed[1] = false;
                    }

                    if (!(btn2 & PAD_CROSS))
                    {
                        if (!controller.cross_pressed[1])
                        {
                            sound.PlaySFX(&snd, 0, 0xA1BA);
                            controller.cross_pressed[1] = true;
                        }
                    }
                    else
                    {
                        controller.cross_pressed[1] = false;
                    }

                    if (!(btn2 & PAD_SQUARE))
                    {
                        if (!controller.mus_pressed[1])
                        {
                            sound.StopMusic();
                            u_long *file;
                            if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                            {
                                sound.LoadMusic(file, 14, 2);
                                sound.PlayMusic();
                                free(file);
                            }
                            cdrom.CDROM_Standby();
                            controller.mus_pressed[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_CIRCLE))
                    {
                        if (!controller.mus_pressed[1])
                        {
                            sound.StopMusic();
                        }
                    }
                    else
                    {
                        controller.mus_pressed[1] = false;
                    }

                    if (!(btn2 & PAD_SELECT))
                    {
                        if (!controller.save_pressed[1])
                        {
                            memcard.MemCard_Save(0x10, player2.x, player2.y, region.REGION_CODE);
                            controller.save_pressed[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_START))
                    {
                        if (!controller.save_pressed[1])
                        {
                            SAVEDATA data = memcard.MemCard_Load(0x10, region.REGION_CODE);
                            player2.x = data.data[0];
                            player2.y = data.data[1];
                            controller.save_pressed[1] = true;
                        }
                    }
                    else
                    {
                        controller.save_pressed[1] = false;
                    }
                }
                else if (controller.CheckType(0x20) == 0x7)
                {
                    graph.nextpri = player2.DrawSprite(graph.ot[graph.db], graph.nextpri);

                    u_short btn2 = controller.CheckButton(0x20);
                    int rs_x = (int)controller.CheckStick(0x20, 0) - 127;
                    int rs_y = (int)controller.CheckStick(0x20, 1) - 127;
                    int ls_x = (int)controller.CheckStick(0x20, 2) - 127;
                    int ls_y = (int)controller.CheckStick(0x20, 3) - 127;

                    if (ls_y < -15)
                    {
                        player2.y -= -(ls_y / 15);
                    }
                    if (ls_y > 15)
                    {
                        player2.y += ls_y / 15;
                    }
                    if (ls_x < -15)
                    {
                        player2.x -= -(ls_x / 15);
                    }
                    if (ls_x > 15)
                    {
                        player2.x += ls_x / 15;
                    }

                    if (!(btn2 & PAD_UP) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player2.y -= 4;
                    }
                    else if (!(btn2 & PAD_DOWN) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player2.y += 4;
                    }
                    if (!(btn2 & PAD_LEFT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player2.x -= 4;
                    }
                    else if (!(btn2 & PAD_RIGHT) && (ls_x >= -15 && ls_x <= 15) && (ls_y >= -15 && ls_y <= 15))
                    {
                        player2.x += 4;
                    }

                    if (!(btn2 & PAD_R1))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 1;
                            controller.motor[1][1] = 0;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_R2))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 0;
                            controller.motor[1][1] = 0;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_L1))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 0;
                            controller.motor[1][1] = 255;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_L2))
                    {
                        if (!controller.vib_pressed[1])
                        {
                            controller.motor[1][0] = 0;
                            controller.motor[1][1] = 0;
                            controller.vib_pressed[1] = true;
                            controller.vib_sync[1] = true;
                        }
                    }
                    else
                    {
                        controller.vib_pressed[1] = false;
                    }

                    if (!(btn2 & PAD_CROSS))
                    {
                        if (!controller.cross_pressed[1])
                        {
                            sound.PlaySFX(&snd, 0, 0xA1BA);
                            controller.cross_pressed[1] = true;
                        }
                    }
                    else
                    {
                        controller.cross_pressed[1] = false;
                    }

                    if (!(btn2 & PAD_SQUARE))
                    {
                        if (!controller.mus_pressed[1])
                        {
                            sound.StopMusic();
                            u_long *file;
                            if ((file = cdrom.CDROM_ReadFile("\\DATA\\MUS\\MUSIC.MUS;1")))
                            {
                                sound.LoadMusic(file, 14, 2);
                                sound.PlayMusic();
                                free(file);
                            }
                            cdrom.CDROM_Standby();
                            controller.mus_pressed[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_CIRCLE))
                    {
                        if (!controller.mus_pressed[1])
                        {
                            sound.StopMusic();
                        }
                    }
                    else
                    {
                        controller.mus_pressed[1] = false;
                    }

                    if (!(btn2 & PAD_SELECT))
                    {
                        if (!controller.save_pressed[1])
                        {
                            memcard.MemCard_Save(0x10, player2.x, player2.y, region.REGION_CODE);
                            controller.save_pressed[1] = true;
                        }
                    }
                    else if (!(btn2 & PAD_START))
                    {
                        if (!controller.save_pressed[1])
                        {
                            SAVEDATA data = memcard.MemCard_Load(0x10, region.REGION_CODE);
                            player2.x = data.data[0];
                            player2.y = data.data[1];
                            controller.save_pressed[1] = true;
                        }
                    }
                    else
                    {
                        controller.save_pressed[1] = false;
                    }
                }
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

    if (player2.x < 0)
    {
        player2.x = 0;
    }
    else if (player2.x > graph.ResW - 64)
    {
        player2.x = graph.ResW - 64;
    }

    if (player2.y < 0)
    {
        player2.y = 0;
    }
    else if (player2.y > graph.ResH - 64)
    {
        player2.y = graph.ResH - 64;
    }

    graph.GraphDisp();
}