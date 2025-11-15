#include "controller.hpp"

GameController *GameController::instance = nullptr;

GameController::GameController()
{
    align[0] = 0;
    align[1] = 1;
    align[2] = 0xFF;
    align[3] = 0xFF;
    align[4] = 0xFF;
    align[5] = 0xFF;

    motor[0][0] = 0;
    motor[0][1] = 0;
    motor[1][0] = 0;
    motor[1][1] = 0;

    cross_pressed[0] = false;
    vib_pressed[0] = false;
    save_pressed[0] = false;
    mus_pressed[0] = false;
    xa_pressed[0] = false;

    cross_pressed[1] = false;
    vib_pressed[1] = false;
    save_pressed[1] = false;
    mus_pressed[1] = false;
    xa_pressed[1] = false;

    vib_sync[0] = false;
    vib_sync[1] = false;
}

GameController::~GameController()
{
    
}

void GameController::ControllerInit()
{
    PadInitMtap(instance->padbuff[0], instance->padbuff[1]);
    PadStartCom();
}

int GameController::IsConnected(int port)
{
    u_char *buff = instance->padbuff[0];
    u_char *buff2 = instance->padbuff[1];

    if (port == 0x00)
    {
        if (buff[0] == 0)
        {
            return 1;
        }
    }
    if (port == 0x10)
    {
        if (buff[2] == 0)
        {
            return 1;
        }
    }
    if (port == 0x11)
    {
        if (buff[10] == 0)
        {
            return 1;
        }
    }
    if (port == 0x12)
    {
        if (buff[18] == 0)
        {
            return 1;
        }
    }
    if (port == 0x13)
    {
        if (buff[26] == 0)
        {
            return 1;
        }
    }
    if (port == 0x01)
    {
        if (buff2[0] == 0)
        {
            return 1;
        }
    }
    if (port == 0x20)
    {
        if (buff2[2] == 0)
        {
            return 1;
        }
    }
    if (port == 0x21)
    {
        if (buff2[10] == 0)
        {
            return 1;
        }
    }
    if (port == 0x22)
    {
        if (buff2[18] == 0)
        {
            return 1;
        }
    }
    if (port == 0x23)
    {
        if (buff2[26] == 0)
        {
            return 1;
        }
    }

    return 0;
}

u_char GameController::CheckType(int port)
{
    u_char *buff = instance->padbuff[0];
    u_char *buff2 = instance->padbuff[1];

    if (port == 0x00)
    {
        return buff[1] >> 4;
    }
    if (port == 0x10)
    {
        return buff[3] >> 4;
    }
    if (port == 0x11)
    {
        return buff[11] >> 4;
    }
    if (port == 0x12)
    {
        return buff[19] >> 4;
    }
    if (port == 0x13)
    {
        return buff[27] >> 4;
    }
    if (port == 0x01)
    {
        return buff2[1] >> 4;
    }
    if (port == 0x20)
    {
        return buff2[3] >> 4;
    }
    if (port == 0x21)
    {
        return buff2[11] >> 4;
    }
    if (port == 0x22)
    {
        return buff2[19] >> 4;
    }
    if (port == 0x23)
    {
        return buff2[27] >> 4;
    }

    return 0x0;
}

u_short GameController::CheckButton(int port)
{
    u_char *buff = padbuff[0];
    u_char *buff2 = padbuff[1];

    if (port == 0x00)
    {
        return *((u_short *)(buff + 2));
    }
    if (port == 0x10)
    {
        return *((u_short *)(buff + 4));
    }
    if (port == 0x11)
    {
        return *((u_short *)(buff + 12));
    }
    if (port == 0x12)
    {
        return *((u_short *)(buff + 20));
    }
    if (port == 0x13)
    {
        return *((u_short *)(buff + 28));
    }
    if (port == 0x01)
    {
        return *((u_short *)(buff2 + 2));
    }
    if (port == 0x20)
    {
        return *((u_short *)(buff2 + 4));
    }
    if (port == 0x21)
    {
        return *((u_short *)(buff2 + 12));
    }
    if (port == 0x22)
    {
        return *((u_short *)(buff2 + 20));
    }
    if (port == 0x23)
    {
        return *((u_short *)(buff2 + 28));
    }

    return 0;
}

u_char GameController::CheckStick(int port, int stick)
{
    u_char *buff = instance->padbuff[0];
    u_char *buff2 = instance->padbuff[1];

    if (port == 0x00)
    {
        return buff[4 + stick];
    }
    if (port == 0x10)
    {
        return buff[6 + stick];
    }
    if (port == 0x11)
    {
        return buff[14 + stick];
    }
    if (port == 0x12)
    {
        return buff[22 + stick];
    }
    if (port == 0x13)
    {
        return buff[30 + stick];
    }
    if (port == 0x01)
    {
        return buff2[4 + stick];
    }
    if (port == 0x20)
    {
        return buff2[6 + stick];
    }
    if (port == 0x21)
    {
        return buff2[14 + stick];
    }
    if (port == 0x22)
    {
        return buff2[22 + stick];
    }
    if (port == 0x23)
    {
        return buff2[30 + stick];
    }

    return 0;
}

void GameController::LoopVibrator(int port)
{
    int index = 0;

    switch(port)
    {
        case 0x00:
        {
            index = 0;
            break;
        }
        case 0x01:
        {
            index = 1;
            break;
        }
        case 0x02:
        {
            index = 2;
            break;
        }
        case 0x03:
        {
            index = 3;
            break;
        }
        case 0x10:
        {
            index = 1;
        }
    }

    if (vib_sync[index])
    {
        if (PadGetState(port) == PadStateStable)
        {
            while (PadSetActAlign(port, align) != 0)
            {
                VSync(0);
            }

            PadSetAct(port, motor[index], sizeof(motor[index]));
        }

        vib_sync[index] = false;
    }
}