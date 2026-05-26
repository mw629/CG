#include "PauseSystem.h"

void PauseSystem::Update()
{

}

void PauseSystem::Draw()
{
	Engine::ChangePostEffect(Engine::PostEffectType::Smoothing);
}
