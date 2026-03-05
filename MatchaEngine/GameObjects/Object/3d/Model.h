#pragma once
#include"ObjectBase.h"


class Model	:public ObjectBase
{
private:
	int modelNumber_;

	//アニメーション
	Node rootNode_;

public:
	
	~Model()override;


	void Initialize(ModelData modelData);

	void SettingWvp(Matrix4x4 viewMatrix) override;
	void CreateIndexResource()override;
	
	void CreateObject()override;

};

