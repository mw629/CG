#pragma once
#include"ObjectBase.h"


class Model	:public ObjectBase
{
private:


	//アニメーション

public:
	
	~Model()override;


	void Initialize(int modelData);

	void SettingWvp(Matrix4x4 viewMatrix) override;
	
	void CreateObject()override;

	

};

