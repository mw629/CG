#include "Engine.h"

Engine::~Engine()
{
	delete graphicsDevice;
}

Engine::Engine()
{
	logStream = CurrentTimestamp();
	graphicsDevice = new GraphicsDevice(logStream);
}
