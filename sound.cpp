#include "sound.hpp"

GameSound::GameSound()
{

}

GameSound::~GameSound()
{

}

void GameSound::SoundInit()
{
    SpuCommonAttr commonAttributes;
    SpuInit();
    SpuInitMalloc(MALLOC_MAX, spu_malloc_rec);
    commonAttributes.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR);
    commonAttributes.mvol.left  = 0x3fff;
    commonAttributes.mvol.right = 0x3fff;
    commonAttributes.cd.volume.left = 0x3fff;
    commonAttributes.cd.volume.right = 0x3fff;
    SpuSetCommonAttr(&commonAttributes);
    SpuSetIRQ(SPU_OFF);
}

u_long GameSound::SendVAGtoSPU(unsigned int VAG_data_size, u_char *VAG_data)
{
    u_long size;
    SpuSetTransferMode(SpuTransByDMA);
    size = SpuWrite (VAG_data + sizeof(VAGhdr), VAG_data_size);
    SpuIsTransferCompleted (SPU_TRANSFER_WAIT);
    return size;
}

void GameSound::SetVoiceAttr(unsigned int pitch, long channel, u_long soundAddr)
{
    SpuVoiceAttr  voiceAttributes;

    voiceAttributes.mask=
    (
        SPU_VOICE_VOLL |
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
        SPU_VOICE_ADSR_SL
    );
    voiceAttributes.voice        = channel;
    voiceAttributes.volume.left  = 0x0;
    voiceAttributes.volume.right = 0x0;
    voiceAttributes.pitch        = pitch;
    voiceAttributes.addr         = soundAddr;
    voiceAttributes.a_mode       = SPU_VOICE_LINEARIncN;
    voiceAttributes.s_mode       = SPU_VOICE_LINEARIncN;
    voiceAttributes.r_mode       = SPU_VOICE_LINEARDecN;
    voiceAttributes.ar           = 0x0;
    voiceAttributes.dr           = 0x0;
    voiceAttributes.rr           = 0x0;
    voiceAttributes.sr           = 0x0;
    voiceAttributes.sl           = 0xf;
    SpuSetVoiceAttr(&voiceAttributes);
}

u_long GameSound::SetSPUtransfer(VAGsound *sound)
{
    u_long transferred, spu_address;
    const VAGhdr * VAGheader = (VAGhdr *) sound->VAGfile;
    sound->pitch = (SWAP_ENDIAN32(VAGheader->samplingFrequency) << 12) / 44100L;
    spu_address = SpuMalloc(SWAP_ENDIAN32(VAGheader->dataSize));
    SpuSetTransferStartAddr(spu_address);
    transferred = SendVAGtoSPU(SWAP_ENDIAN32(VAGheader->dataSize), sound->VAGfile);
    SetVoiceAttr(sound->pitch, sound->spu_channel, spu_address);

    return spu_address;
}

void GameSound::PlaySFX(VAGsound *sound)
{
    SpuVoiceAttr  voiceAttributes;
    
    voiceAttributes.mask= ( SPU_VOICE_VOLL | SPU_VOICE_VOLR );
    voiceAttributes.voice        = sound->spu_channel;
    voiceAttributes.volume.left  = 0x1000;
    voiceAttributes.volume.right = 0x1000;
    SpuSetVoiceAttr(&voiceAttributes);

    SpuSetKey(SpuOn, sound->spu_channel);
}