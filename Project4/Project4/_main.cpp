#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../../Common/Utility/Console.h"
#include "Engine.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1);

	Con::SetPosition(-965, 0);
	Con::SetSize(965, 1045);

	Engine engine;
	engine.Init();
	engine.Loop();
	engine.ShutDown();

	return 0;
}