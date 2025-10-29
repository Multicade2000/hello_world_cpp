#include "game.hpp"
#include <malloc.h>

static unsigned char ramAddr[0x40000];

int main()
{
	GameEngine engine = GameEngine();
	engine.GameInit();

	InitHeap((u_long *)ramAddr, sizeof(ramAddr));

	engine.GameLoadStuff();

	while(1)
	{
		engine.GameLoop();
	}

	return 0;
}