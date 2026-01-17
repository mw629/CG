#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <vector>

class DirectionalLight;
class SpotLight;
class PointLight;

struct LightCount {
	int pointLightCount;
	int spotLightCount;
	int padding[2];
};

struct LightData {
	// 各ライトタイプの配列用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightsResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightsResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightsResource_;
	//
	LightCount lightCount;
};

class LightManager
{
private:

	LightData lightData_;

public:


	// ライト登録メソッド
	static void RegisterLight(DirectionalLight* light);
	static void RegisterLight(PointLight* light);
	static void RegisterLight(SpotLight* light);

	// ライトのクリア（フレームの最初に呼ぶ）
	void ClearLights();

	// GPU用のライトデータを更新
	void UpdateLightData();

	// 描画時にコマンドリストにセット
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);


};

