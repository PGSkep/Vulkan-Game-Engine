#ifndef	ENGINE_H
#define ENGINE_H

#include "GpuManager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"

#include "../../Common/Utility/Clock.h"
#include "../../Common/Utility/Input.h"

class Engine
{
	GpuManager		gpuManager;
	SceneManager	sceneManager;
	PhysicsManager	physicsManager;

public:
	void Init()
	{
		Clock::Init();

		Input::Init();
		Input::Reset();
		Input::Update();

		gpuManager.Init();
		sceneManager.Init();
		physicsManager.Init();
	}
	void Loop()
	{
		while (!Input::IsDown(Input::ESC))
		{
			Input::Update();
			Clock::Update();

			gpuManager.Run();
		}
	}
	void ShutDown()
	{
		gpuManager.ShutDown();
		sceneManager.ShutDown();
		physicsManager.ShutDown();

		Input::ShutDown();
	}
};

#endif