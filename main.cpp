#include "game.hpp"
#include <malloc.h>

int main()
{
	GameEngine engine = GameEngine();
	engine.GameInit();

	engine.GameLoadStuff();

	while(1)
	{
		engine.GameLoop();
	}

	return 0;
}