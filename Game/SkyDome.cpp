#include "SkyDome.h"

SkyDome::~SkyDome()
{
	delete skydomeModel;
	delete fogModel;

}

void SkyDome::Initialize() {
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	ModelData modelData = LoadObjFile("resources/skydome", "skydome.obj");
	skydomeModel = new Model();
	skydomeModel->Initialize(modelData);
	skydomeModel->CreateModel();


	modelData = LoadObjFile("resources/skydome", "skydome.obj");
	int index = texture->CreateTexture("resources/skydome/skydome.png");
	fogModel = new Model();
	fogModel->Initialize(modelData);
	fogModel->SetTexture(texture.get()->TextureData(index));
	fogModel->CreateModel();
	fogModel->GetMatrial()->SetColor({ 1.0f,1.0f,1.0f,0.2f });
	fogModel->SetBlend(kBlendModeNormal);

	//生成
	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	camera_.get()->SetTransform(transform_);
	camera_.get()->Update();
}

void SkyDome::Update(Matrix4x4 viewMatrix) {
	skydomeModel->SetTransform(objTransform);
	skydomeModel->SettingWvp(viewMatrix);

	fogTransform.rotate.y += 3.14 / (60.0f * 10.0f);

	fogModel->SetTransform(fogTransform);
	fogModel->SettingWvp(viewMatrix);
}

void SkyDome::Update(int stage) {
	skydomeModel->SetTransform(objTransform);
	skydomeModel->SettingWvp(camera_.get()->GetViewMatrix());
	if (stage == 9) {
		skydomeModel->GetMatrial()->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	}
	fogTransform.rotate.y += 3.14 / (60.0f * 10.0f);

	fogModel->SetTransform(fogTransform);
	fogModel->SettingWvp(camera_.get()->GetViewMatrix());
}

void SkyDome::Update() {

	skydomeModel->SetTransform(objTransform);
	skydomeModel->SettingWvp(camera_.get()->GetViewMatrix());

	fogTransform.rotate.y += 3.14 / (60.0f * 10.0f);

	fogModel->SetTransform(fogTransform);
	fogModel->SettingWvp(camera_.get()->GetViewMatrix());
}

void SkyDome::Draw() {
	Draw::DrawModel(skydomeModel);
	Draw::DrawModel(fogModel);
}