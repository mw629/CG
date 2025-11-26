#include "GameManager.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "StageSelect/StageSelect.h"
#include "ClearScene/ClearScene.h"
#include "GameOverScene/GameOverScene.h"

GameManager::GameManager()
{
	scene_ = new GameScene();
}

GameManager::~GameManager()
{
	delete scene_;
}

void GameManager::ImGui() {
#ifdef DEBUG
	scene_->ImGui();
#endif // DEBUG
}

void GameManager::Initialize() {

	mapManager_ = std::make_unique<MapManager>();
	scene_->Initialize();

	fade_ = std::make_unique<Fade>();
	fade_.get()->Initialize(0.5f);

	isChange_ = false;
}

void GameManager::Update() {

	int NexrScene = scene_->GetNextSceneID();

	if (scene_->GetSceneChangeRequest()) {
		if (!isChange_) {
			fade_.get()->FadeIn();
		}
		isChange_ = true;
	}

	if (isChange_) {
		if (fade_.get()->IsFinish()) {
			isChange_ = false;
			delete  scene_;
			scene_ = CreateScene(NexrScene);
			scene_->Initialize();
			scene_->Update();
			fade_.get()->FadeOut();
		}
	}

	fade_.get()->Update();

	if (!isChange_) {
		scene_->Update();
	}

}
void GameManager::Draw() {
	scene_->Draw();
	fade_.get()->Draw();
}

IScene* GameManager::CreateScene(int sceneID)
{
	switch (sceneID) {
	case SceneID::kTitle: return new TitleScene();
	case SceneID::kStageSelect: return new StageSelect();
	case SceneID::kGame:  return new GameScene();
	case SceneID::kClear: return new ClearScene();
	case SceneID::kGameOver: return new GameOverScene();
	default: return nullptr;
	}
}
