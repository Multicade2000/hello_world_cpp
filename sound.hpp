#include <sys/types.h>
#include <stdio.h>
#define _WCHAR_T
#include <stdlib.h>
#include <libapi.h>
#include <libetc.h>
#include <libspu.h>

#define MALLOC_MAX 256
#define SWAP_ENDIAN32(x) (((x)>>24) | (((x)>>8) & 0xFF00) | (((x)<<8) & 0x00FF0000) | ((x)<<24))

typedef struct VAGsound {
    u_long spu_address;
    u_int pitch;
} VAGsound;

typedef struct VAGheader{
    char id[4];
    unsigned int version;
    unsigned int reserved;
    unsigned int dataSize;
    unsigned int samplingFrequency;
    char  reserved2[12];
    char  name[16];

} VAGhdr;

typedef struct MUSheader {
    u_char sampleid;
    u_char channel;
    u_short note_key;
    u_short time;
    u_char loopStart;
    u_char loopEnd;
} MusHdr;

class GameSound
{
public:
    GameSound();
    ~GameSound();

    u_short mus_tick[12];

    static GameSound* instance;

    VAGsound mus[254];
    MusHdr *muser;

    int chan_ofs[12];
    int chan_loop[12];
    bool chan_start[12];
    bool chan_stop[12];

    bool mus_playing = false;

    int stopper;

    int max_channels;

    int curPos[12];

    long musicEvent;

    char spu_malloc_rec[SPU_MALLOC_RECSIZ * (MALLOC_MAX+1)];

    void SoundInit();
    u_long SendVAGtoSPU(unsigned int VAG_data_size, u_char *VAG_data);
    void SetVoiceAttr(unsigned int pitch, int channel, u_long soundAddr, u_short key);

    u_long SetSPUtransfer(VAGsound *sound, u_long *file);
    void PlaySFX(VAGsound *sound, int channel, u_short key);
    void StopSFX(int channel);

    void LoadMusic(u_long *file, int size, int max_chans);
    void PlayMusic();
    void StopMusic();
    void ClearMusic();
    static long ProcessMusic();
};