#pragma once
#include <Engine.h>

class SkyDome
{
private:

	Model* skydomeModel;
	Transform objTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	
	Model* fogModel;
	Transform fogTransform = { {0.99f, 0.99f, 0.99f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };

	std::unique_ptr<Camera> camera_;
	Transform transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{12.0f,0.0f,-50.0f} };

public:
	~SkyDome();

	void Initialize();

	void Update(Matrix4x4 viewMatrix);

	void Update();

	void Draw();


};

