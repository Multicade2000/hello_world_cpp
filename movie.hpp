#include <sys/types.h>
#include <stdio.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libpress.h>
#include <malloc.h>

// Decode environment
typedef struct {
	u_long	*VlcBuff_ptr[2];	// Pointers to the VLC buffers
	u_short	*ImgBuff_ptr[2];	// Pointers to the frame slice buffers
	RECT	rect[2];			// VRAM parameters on where to draw the frame data to
	RECT	slice;				// Frame slice parameters for loading into VRAM
	int		VlcID;				// Current VLC buffer ID
	int		ImgID;				// Current slice buffer ID
	int 	RectID;				// Current video buffer ID
	int		FrameDone;			// Frame decode completion flag
} STRENV;

class Movie
{
public:
    static Movie *instance;

    STRENV strEnv;

    int	strScreenWidth,strScreenHeight;
    int	strFrameX,strFrameY;
    int	strNumFrames;

    int strFrameWidth,strFrameHeight;
    int strPlayDone;

    bool pal;
    bool doubleSpeed;

    DISPENV disp[2];
    DRAWENV draw[2];
    int db;

    Movie();
    ~Movie();

    void PlayStr(const char *filename, int frames, bool eu, bool doubleSpeed);

    static void strDoPlayback(const char *filename);
    static void strCallback();
    static void strNextVlc(STRENV *strEnv);
    static u_long *strNext(STRENV *strEnv);
    static void strKickCD(CdlLOC *loc);
};