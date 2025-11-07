#include "sound.hpp"

GameSound *GameSound::instance = nullptr;

GameSound::GameSound()
{
    for (int i = 0; i < 12; i++)
    {
        mus_tick[i] = 0;
        curPos[i] = 0;
    }

    stopper = 0;

    muser = nullptr;
}

GameSound::~GameSound()
{
    
}

void GameSound::SoundInit()
{
    SpuCommonAttr commonAttributes;
    SpuInit();
    SpuInitMalloc(MALLOC_MAX, spu_malloc_rec);
    SpuWrite0(0xFFFFFF);
    commonAttributes.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);
    commonAttributes.mvol.left = 0x3fff;
    commonAttributes.mvol.right = 0x3fff;
    commonAttributes.cd.volume.left = 0x3fff;
    commonAttributes.cd.volume.right = 0x3fff;
    commonAttributes.cd.mix = SPU_ON;
    SpuSetCommonAttr(&commonAttributes);
    SpuSetIRQ(SPU_OFF);
}

u_long GameSound::SendVAGtoSPU(unsigned int VAG_data_size, u_char *VAG_data)
{
    u_long size;
    SpuSetTransferMode(SpuTransByDMA);
    size = SpuWrite(VAG_data + sizeof(VAGhdr), VAG_data_size);
    SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
    return size;
}

void GameSound::SetVoiceAttr(unsigned int pitch, int channel, u_long soundAddr, u_short key)
{
    SpuVoiceAttr voiceAttributes;

    voiceAttributes.mask =
        (SPU_VOICE_VOLL |
         SPU_VOICE_VOLR |
         SPU_VOICE_PITCH |
         SPU_VOICE_WDSA |
         SPU_VOICE_ADSR_AMODE |
         SPU_VOICE_ADSR_SMODE |
         SPU_VOICE_ADSR_RMODE |
         SPU_VOICE_ADSR_AR |
         SPU_VOICE_ADSR_DR |
         SPU_VOICE_ADSR_SR |
         SPU_VOICE_ADSR_RR |
         SPU_VOICE_ADSR_SL |
         SPU_VOICE_NOTE);
    voiceAttributes.voice = (0x1L << channel);
    voiceAttributes.volume.left = 0x0FFF;
    voiceAttributes.volume.right = 0x0FFF;
    voiceAttributes.pitch = pitch;
    voiceAttributes.addr = soundAddr;
    voiceAttributes.a_mode = SPU_VOICE_LINEARIncN;
    voiceAttributes.s_mode = SPU_VOICE_LINEARIncN;
    voiceAttributes.r_mode = SPU_VOICE_LINEARDecN;
    voiceAttributes.ar = 0x0;
    voiceAttributes.dr = 0x0;
    voiceAttributes.rr = 0x0;
    voiceAttributes.sr = 0x0;
    voiceAttributes.sl = 0xf;
    voiceAttributes.note = key;
    SpuSetVoiceAttr(&voiceAttributes);
}

u_long GameSound::SetSPUtransfer(VAGsound *sound, u_long *file)
{
    u_long transferred, spu_address;
    const VAGhdr *VAGheader = (VAGhdr *)file;
    sound->pitch = (SWAP_ENDIAN32(VAGheader->samplingFrequency) << 12) / 44100L;
    spu_address = SpuMalloc(SWAP_ENDIAN32(VAGheader->dataSize));
    SpuSetTransferStartAddr(spu_address);
    transferred = SendVAGtoSPU(SWAP_ENDIAN32(VAGheader->dataSize), (u_char *)file);
    // SetVoiceAttr(sound->pitch, sound->spu_channel, spu_address);

    return spu_address;
}

void GameSound::PlaySFX(VAGsound *sound, int channel, u_short key)
{
    // SpuVoiceAttr  voiceAttributes;

    // voiceAttributes.mask= ( SPU_VOICE_VOLL | SPU_VOICE_VOLR );
    // voiceAttributes.voice        = sound->spu_channel;
    // voiceAttributes.volume.left  = 0x1000;
    // voiceAttributes.volume.right = 0x1000;
    // SpuSetVoiceAttr(&voiceAttributes);

    SetVoiceAttr(sound->pitch, channel, sound->spu_address, key);

    SpuSetKey(SpuOn, (0x1L << channel));
}

void GameSound::StopSFX(int channel)
{
    SpuSetKey(SpuOff, (0x1L << channel));
}

void GameSound::LoadMusic(u_long *file, int size, int max_chans)
{
    const char *lister = (char *)file;
    int idx = 0;
    int looper = 0;

    max_channels = max_chans;

    muser = (MusHdr *)malloc(size * sizeof(MusHdr));

    for (int i = 0; i < 8 * size; i += 8)
    {
        muser[idx].sampleid = (u_char)lister[i + 0];
        muser[idx].channel = (u_char)lister[i + 1];
        muser[idx].note_key = ((u_char)lister[i + 2] << 8) | (u_char)lister[i + 3];
        muser[idx].time = ((u_char)lister[i + 5] << 8) | (u_char)lister[i + 4];
        muser[idx].loopStart = (u_char)lister[i + 6];
        muser[idx].loopEnd = (u_char)lister[i + 7];

        for (int j = 0; j < max_chans; j++)
        {
            if (muser[idx].channel == j && !chan_start[j])
            {
                chan_ofs[j] = idx;
                chan_start[j] = true;
                looper = 0;
            }
        }

        if (muser[idx].loopStart == 0x01)
        {
            chan_loop[muser[idx].channel] = looper;
        }

        idx++;
        looper++;
    }

    // return sizeof(file);
}

void GameSound::PlayMusic()
{
    ResetRCnt(RCntCNT1);
    SetRCnt(RCntCNT1, 125, RCntMdINTR);
    StartRCnt(RCntCNT1);
    musicEvent = OpenEvent(RCntCNT1, EvSpINT, EvMdINTR, GameSound::ProcessMusic);
    EnableEvent(musicEvent);
}

void GameSound::StopMusic()
{
    if (muser)
    {
        StopRCnt(RCntCNT1);
        DisableEvent(musicEvent);
        CloseEvent(musicEvent);
        for (int i = 0; i < 12; i++)
        {
            StopSFX(i);
            curPos[i] = 0;
            mus_tick[i] = 0;
            chan_ofs[i] = 0;
            chan_start[i] = false;
            chan_loop[i] = 0;
            VSync(0);
        }
        free(muser);
        muser = nullptr;
    }
}

long GameSound::ProcessMusic()
{
    if (instance)
    {
        for (int i = 0; i < instance->max_channels; i++)
        {
            if (instance->mus_tick[i] < instance->muser[instance->chan_ofs[i] + instance->curPos[i]].time)
            {
                if (instance->mus_tick[i] == 0)
                {
                    instance->PlaySFX(&instance->mus[instance->muser[instance->chan_ofs[i] + instance->curPos[i]].sampleid], instance->muser[instance->chan_ofs[i] + instance->curPos[i]].channel, instance->muser[instance->chan_ofs[i] + instance->curPos[i]].note_key);
                }
                instance->mus_tick[i]++;
            }
            else
            {
                if (i == instance->stopper)
                {
                    instance->StopSFX(instance->muser[instance->chan_ofs[i] + instance->curPos[i]].channel);
                }
                instance->mus_tick[i] = 0;
                if (instance->muser[instance->chan_ofs[i] + instance->curPos[i]].loopEnd == 0x01)
                {
                    instance->curPos[i] = instance->chan_loop[i];
                }
                else
                {
                    instance->curPos[i]++;
                    if (instance->muser[instance->chan_ofs[i] + instance->curPos[i]].channel != i)
                    {
                        instance->curPos[i] = instance->chan_loop[i];
                    }
                }
            }
        }
    }

    if (instance->stopper < instance->max_channels - 1)
    {
        instance->stopper++;
    }
    else
    {
        instance->stopper = 0;
    }

    SetRCnt(RCntCNT1, 125, RCntMdINTR);

    return 125;
}