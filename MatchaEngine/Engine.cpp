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

void Engine::Setting()
{

}

void Engine::PreDraw()
{

}

void Engine::PostDraw()
{
}
