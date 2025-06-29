#pragma once
#include "Common/LogHandler.h"
#include "Common/GraphicsDevice.h"
#include <fstream>

class Engine
{
private:
	std::ofstream logStream;
	GraphicsDevice* graphicsDevice;
public:

	~Engine();
	Engine();

	void Setting();

	void PreDraw();

	void PostDraw();


};

