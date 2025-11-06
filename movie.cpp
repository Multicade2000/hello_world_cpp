#include "movie.hpp"
#include "controller.hpp"

Movie *Movie::instance = nullptr;

Movie::Movie()
{
	db = 0;
}

Movie::~Movie()
{

}

void Movie::PlayStr(const char *filename, int frames, bool eu, bool dS)
{
	instance->doubleSpeed = dS;
	instance->pal = eu;

	instance->strNumFrames = frames;
	instance->strScreenWidth = 320;
	instance->strScreenHeight = (instance->pal ? 256 : 240);
	instance->strFrameX = 0;
	instance->strFrameY = (instance->pal ? 8 : 0);

	instance->strPlayDone = 0;
	instance->strDoPlayback(filename);
}

void Movie::strDoPlayback(const char *filename)
{
	if (instance)
	{
		int id;		  // Display buffer ID
		DISPENV disp; // Display environment
		CdlFILE file; // File info of video file

		// Buffers initialized here so we won't waste too much memory for playing FMVs
		// (just make sure you have at least 192KB of free memory before calling this routine)
		u_long RingBuff[32 * SECTOR_SIZE];	  // Ring buffer
		u_long VlcBuff[2][320 / 2 * (instance->pal ? 256 : 240)]; // VLC buffers
		u_short ImgBuff[2][16 * 1 * (instance->pal ? 256 : 240)];	  // Frame 'slice' buffers

		// Set display mask so we won't see garbage while the stream is being prepared
		SetDispMask(0);

		// Get the CD location of the STR file to play
		if (CdSearchFile(&file, (char *)filename) == 0)
		{
			return;
		}

		// Setup the buffer pointers
		instance->strEnv.VlcBuff_ptr[0] = &VlcBuff[0][0];
		instance->strEnv.VlcBuff_ptr[1] = &VlcBuff[1][0];
		instance->strEnv.VlcID = 0;
		instance->strEnv.ImgBuff_ptr[0] = &ImgBuff[0][0];
		instance->strEnv.ImgBuff_ptr[1] = &ImgBuff[1][0];
		instance->strEnv.ImgID = 0;

		// Setup the display buffers on VRAM
		instance->strEnv.rect[0].x = instance->strFrameX; // First page
		instance->strEnv.rect[0].y = instance->strFrameY;
		instance->strEnv.rect[1].x = instance->strFrameX; // Second page
		instance->strEnv.rect[1].y = instance->strFrameY + instance->strScreenHeight;
		instance->strEnv.RectID = 0;

		// Set the parameters for uploading frame slices
		instance->strEnv.slice.x = instance->strFrameX;
		instance->strEnv.slice.y = instance->strFrameY;
		instance->strEnv.slice.w = 16 * 1;
		instance->strEnv.FrameDone = 0;

		// Reset the MDEC
		DecDCTReset(0);
		// Set callback routine
		DecDCToutCallback(instance->strCallback);
		// Set ring buffer
		StSetRing(RingBuff, 32);
		// Set streaming parameters
		StSetStream(0, 1, 0xffffffff, 0, 0);
		// Begin streaming!
		instance->strKickCD(&file.pos);

		// Load the first frame of video before entering main loop
		instance->strNextVlc(&instance->strEnv);

		while (1)
		{

			// Decode the compressed frame data
			DecDCTin(instance->strEnv.VlcBuff_ptr[instance->strEnv.VlcID], 2);

			// Prepare to receive the decoded image data from the MDEC
			DecDCTout((u_long *)instance->strEnv.ImgBuff_ptr[instance->strEnv.ImgID], instance->strEnv.slice.w * instance->strEnv.slice.h / 2);

			// Get the next frame
			instance->strNextVlc(&instance->strEnv);

			// Wait for the frame to finish decoding
			// strSync(&strEnv, 0);

			// Switch between the display buffers per frame
			id = instance->strEnv.RectID ? 0 : 1;
			SetDefDispEnv(&disp, 0, instance->strScreenHeight * id, instance->strScreenWidth * 1, instance->strScreenHeight);

			if (instance->pal)
			{
				disp.screen.y = 24;
			}

			// if (instance->bpp24)
			// {
			// 	disp.isrgb24 = 1;
			// 	disp.disp.w = disp.disp.w * 2 / 3;
			// }

			VSync(0);		   // VSync to avoid screen tearing
			PutDispEnv(&disp); // Apply the video parameters
			SetDispMask(1);	   // Remove the display mask

			if (instance->strPlayDone == 1)
			{
				break;
			}

			if (GameController::instance->IsConnected(0x00))
			{
				if (GameController::instance->CheckType(0x00) == 0x4)
				{
					u_short btn = GameController::instance->CheckButton(0x00);

					if (!(btn & PAD_CROSS) || !(btn & PAD_START))
					{
						break;
					}
				}
				if (GameController::instance->CheckType(0x00) == 0x7)
				{
					u_short btn = GameController::instance->CheckButton(0x00);

					if (!(btn & PAD_CROSS) || !(btn & PAD_START))
					{
						break;
					}
				}
				if (GameController::instance->CheckType(0x00) == 0x8)
				{
					if (GameController::instance->IsConnected(0x10))
					{
						if (GameController::instance->CheckType(0x10) == 0x4)
						{
							u_short btn = GameController::instance->CheckButton(0x10);

							if (!(btn & PAD_CROSS) || !(btn & PAD_START))
							{
								break;
							}
						}
						if (GameController::instance->CheckType(0x10) == 0x7)
						{
							u_short btn = GameController::instance->CheckButton(0x10);

							if (!(btn & PAD_CROSS) || !(btn & PAD_START))
							{
								break;
							}
						}
					}
				}
			}
		}

		// Shutdown streaming
		DecDCToutCallback(0);
		StUnSetRing();
		CdControlB(CdlPause, 0, 0);
	}
}

void Movie::strCallback()
{
	if (instance)
	{
		RECT TransferRect;
		int id;

		// In 24-bit color, StCdInterrupt must be called in every callback
		// if (instance->bpp24)
		// {
		// 	extern u_long StCdIntrFlag;
		// 	if (StCdIntrFlag)
		// 	{
		// 		StCdInterrupt();
		// 		StCdIntrFlag = 0;
		// 	}
		// }

		id = instance->strEnv.ImgID;
		TransferRect = instance->strEnv.slice;

		// Switch slice buffers
		instance->strEnv.ImgID = instance->strEnv.ImgID ? 0 : 1;

		// Step to next slice
		instance->strEnv.slice.x += instance->strEnv.slice.w;

		// Frame not yet decoded completely?
		if (instance->strEnv.slice.x < instance->strEnv.rect[instance->strEnv.RectID].x + instance->strEnv.rect[instance->strEnv.RectID].w)
		{
			// Prepare for next slice
			DecDCTout((u_long *)instance->strEnv.ImgBuff_ptr[instance->strEnv.ImgID], instance->strEnv.slice.w * instance->strEnv.slice.h / 2);
		}
		else
		{ // Frame has been decoded completely
			// Set the FrameDone flag
			instance->strEnv.FrameDone = 1;

			// Switch display buffers
			instance->strEnv.RectID = instance->strEnv.RectID ? 0 : 1;
			instance->strEnv.slice.x = instance->strEnv.rect[instance->strEnv.RectID].x;
			instance->strEnv.slice.y = instance->strEnv.rect[instance->strEnv.RectID].y;
		}

		// Transfer the slice into VRAM
		LoadImage(&TransferRect, (u_long *)instance->strEnv.ImgBuff_ptr[id]);
	}
}

void Movie::strNextVlc(STRENV *strEnv)
{
	if (instance)
	{
		int cnt = WAIT_TIME;
		u_long *next = nullptr;
		u_long *strNext();

		// Grab a frame from the stream
		while ((next = instance->strNext(strEnv)) == nullptr)
		{
			if (--cnt == 0)
			{
				// Timeout: nothing received within limit
				return;
			}
		}

		// Switch VLC buffers
		strEnv->VlcID = strEnv->VlcID ? 0 : 1;

		// Decode the VLC
		DecDCTvlc(next, strEnv->VlcBuff_ptr[strEnv->VlcID]);

		// Free the ring buffer
		StFreeRing(next);
	}
}

u_long *Movie::strNext(STRENV *strEnv)
{
	if (instance)
	{
		u_long *addr;
		StHEADER *sector;
		int cnt = WAIT_TIME;

		// Grab a frame
		while (StGetNext((u_long **)&addr, (u_long **)&sector))
		{

			if (--cnt == 0) // Timeout handler
				return (0);
		}

		// If the frame's number has reached number of frames the video has,
		// set the strPlayDone flag.
		if (sector->frameCount >= instance->strNumFrames)
			instance->strPlayDone = 1;

		// if the resolution is differ to previous frame, clear frame buffer
		if (instance->strFrameWidth != sector->width || instance->strFrameHeight != sector->height)
		{

			RECT rect;
			setRECT(&rect, 0, 0, instance->strScreenWidth * 1, instance->strScreenHeight * 2);
			ClearImage(&rect, 0, 0, 0);

			instance->strFrameWidth = sector->width;
			instance->strFrameHeight = sector->height;
		}

		// set STRENV according to the data on the STR format
		strEnv->rect[0].w = strEnv->rect[1].w = instance->strFrameWidth * 1;
		strEnv->rect[0].h = strEnv->rect[1].h = instance->strFrameHeight;
		strEnv->slice.h = instance->strFrameHeight;

		return (addr);
	}

	return nullptr;
}

void Movie::strKickCD(CdlLOC *loc)
{
	if (instance)
	{
		if (instance->doubleSpeed)
		{
			u_char param = CdlModeSpeed;

			bool loop = true;

			while (loop)
			{
				// Seek to the STR file to play
				while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0)
					;
				while (CdControl(CdlSetmode, &param, 0) == 0)
					;

				VSync(3); // Wait for 3 screen cycles before changing drive speed

				// Start streaming
				if (CdRead2(CdlModeStream | CdlModeSpeed | CdlModeRT) != 0)
					loop = false;
			}
		}
		else
		{
			u_char param = 0x00;

			bool loop = true;

			while (loop)
			{
				// Seek to the STR file to play
				while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0)
					;
				while (CdControl(CdlSetmode, &param, 0) == 0)
					;

				VSync(3); // Wait for 3 screen cycles before changing drive speed

				// Start streaming
				if (CdRead2(CdlModeStream | CdlModeRT) != 0)
					loop = false;
			}
		}
	}
}