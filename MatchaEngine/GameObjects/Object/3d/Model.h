#pragma once
#include"ObjectBase.h"


class Model	:public ObjectBase
{
private:
	ModelData modelData_{};
	

public:
	
	~Model()override;


	void Initialize(ModelData modelData);

	void CreateVertexData()override;
	void SettingWvp(Matrix4x4 viewMatrix) override;


	ModelData GetModelData() { return modelData_; }
	


};

