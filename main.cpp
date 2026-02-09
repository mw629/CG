#include "Game/GameApplication.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameApplication> gameApplication_ = std::make_unique<GameApplication>(1280,720);

	gameApplication_.get()->Run();

	return 0;
}