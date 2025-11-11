#include "SkyDome.h"

SkyDome::~SkyDome()
{
	delete skydomeModel;
}

void SkyDome::Initialize() {
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	ModelData modelData = LoadObjFile("resources/skydome", "skydome.obj");
	int index = texture->CreateTexture(modelData.material.textureDilePath);
	skydomeModel = new Model();
	skydomeModel->Initialize(modelData);
	skydomeModel->CreateModel();

	//生成
	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	camera_.get()->SetTransform(transform_);
	camera_.get()->Update();
}

void SkyDome::Update(Matrix4x4 viewMatrix){
	skydomeModel->SetTransform(objTransform);
	skydomeModel->SettingWvp(viewMatrix);
}

void SkyDome::Update() {
	
	skydomeModel->SetTransform(objTransform);
	skydomeModel->SettingWvp(camera_.get()->GetViewMatrix());
}

void SkyDome::Draw(){
	Draw::DrawModel(skydomeModel);
}