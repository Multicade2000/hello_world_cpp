#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libspu.h>

#define MALLOC_MAX 1
#define SWAP_ENDIAN32(x) (((x)>>24) | (((x)>>8) & 0xFF00) | (((x)<<8) & 0x00FF0000) | ((x)<<24))

typedef struct VAGsound {
    u_char * VAGfile;
    u_long spu_channel;
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

}VAGhdr;

class GameSound
{
public:
    GameSound();
    ~GameSound();

    char spu_malloc_rec[SPU_MALLOC_RECSIZ * (MALLOC_MAX+1)];

    void SoundInit();
    u_long SendVAGtoSPU(unsigned int VAG_data_size, u_char *VAG_data);
    void SetVoiceAttr(unsigned int pitch, long channel, u_long soundAddr);

    u_long SetSPUtransfer(VAGsound *sound);
    void PlaySFX(VAGsound *sound);
};