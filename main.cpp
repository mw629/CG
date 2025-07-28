#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <dxgidebug.h>
#include <dbgHelp.h>
#include <strsafe.h>
#include <dxcapi.h>
#include <vector>
#include <wrl.h> 
#include <xaudio2.h>

#include <memory> 

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"

struct Debug {
	~Debug() {
		IDXGIDebug1* debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

#ifdef _DEBUG
std::unique_ptr<Debug> leakChacker = std::make_unique<Debug>();
#endif // _DEBUG


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


#include "MatchaEngine/Engine.h"
#include "MatchaEngine/Input/GamePadInput.h"


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dbgHelp.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"xaudio2.lib")


///クラス///


///-------------------------------------------
///WinMain
///------------------------------------------

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	std::unique_ptr<Engine> engine = std::make_unique<Engine>(1280, 720);
	engine.get()->Setting();

	std::unique_ptr<Draw> draw = std::make_unique<Draw>();

	//テクスチャの作成
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	texture->CreateTexture("resources/uvChecker.png");
	texture->CreateTexture("resources/nightSky.png");
	//線の描画
	std::unique_ptr<Line> line = std::make_unique<Line>();
	LineVertexData lineVertex[2] = { {{-1.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f} },
		{{ 1.0f,1.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }} };
	Transform lineTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	line.get()->CreateLine();
	//グリッド
	std::unique_ptr<Grid> grid = std::make_unique<Grid>();
	grid.get()->CreateGrid();

	//三角形の作成
	std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>();
	triangle->Initialize(texture->TextureData(0));
	triangle->CreateTriangle();
	Transform triangleTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	Vector4 vertex[3] = {
	{ -0.1f, -0.1f, 0.0f, 1.0f },
	{ 0.0f, 0.1f, 0.0f, 1.0f },
	{ 0.1f, -0.1f, 0.0f, 1.0f }
	};
	//スプライト作成
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite->Initialize(texture->TextureData(0));
	sprite->CreateSprite();
	Transform spriteTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	//球の作成
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
	sphere->Initialize(texture->TextureData(0));
	sphere->CreateSprite();
	Transform shpereTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	//モデルの作成
	ModelData modelData = LoadObjFile("resources/obj", "axis.obj");
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelData, texture->TextureData(0));
	model->CreateModel();
	Transform objTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };



	//std::unique_ptr<DirectinalLight> directinalLight = std::make_unique<DirectinalLight>();
	//directinalLight->CreateDirectinalLight(graphics->GetDevice());
	std::unique_ptr<DebugCamera> debugCamera = std::make_unique<DebugCamera>();
	debugCamera.get()->Initialize();

	Matrix4x4 viewMatrix;

	bool debugCameraFlag = false;


	bool useMonsterBall = true;

	Transform camraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

	Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	int BGMHandle;
	audio->Initialize();
	BGMHandle = audio->Load("resources/Audio/BGM/sweet_pop.mp3");
	audio->Play(BGMHandle, false, 1.0f);

	GamePadInput gamepad;


	//メインループ//

	MSG msg{};
	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT) {
		//windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			gamepad.Update();



			engine.get()->NewFrame();

			//ゲーム処理

			gamepad.DrawImGui();
			texture.get()->TextureList();

			//開発用UIの処理。実際に開発用UIを出す場合はここをゲ0無固有の処理に置き換える
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::Checkbox("debugCameraFlag", &debugCameraFlag);
			//ImGui::ShowDemoWindow(); debugCameraFlag

			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("camraTranslate", &camraTransform.translate.x, 0.01f);
				ImGui::DragFloat3("camraRotate", &camraTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("camraScale", &camraTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Triangle")) {
				ImGui::DragFloat3("TranslateTriangle", &triangleTransform.translate.x, 1.0f);
				ImGui::DragFloat3("RotateTriangle", &triangleTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleTriangle", &triangleTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("vertex")) {
				ImGui::DragFloat3("vertex 0", &vertex[0].x, 0.01f);
				ImGui::DragFloat3("vertex 1", &vertex[1].x, 0.01f);
				ImGui::DragFloat3("vertex 2", &vertex[2].x, 0.01f);
			}
			if (ImGui::CollapsingHeader("LVertex")) {
				ImGui::DragFloat3("Lvertex 0", &lineVertex[0].position.x, 0.01f);
				ImGui::DragFloat3("Lvertex 1", &lineVertex[1].position.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("SpriteUV"))
			{
				ImGui::DragFloat2("TranslateSpriteUV", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("ScaleSpriteUV", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("RotateSpriteUV", &uvTransformSprite.rotate.z);
			}


			if (debugCameraFlag) {
				debugCamera->Update(engine.get()->GetInput());
				viewMatrix = debugCamera->GetViewMatrix();
			}
			else
			{
				Matrix4x4 cameraMatrix = MakeAffineMatrix(camraTransform.translate, camraTransform.scale, camraTransform.rotate);;
				viewMatrix = Inverse(cameraMatrix);
			}
			
			sphere.get()->SetMaterialLighting(true);
			model.get()->SetMaterialLighting(true);

			triangle->SetVertex(vertex);
			triangle->SetShape();
			triangle->SetTrandform(triangleTransform);
			triangle->SettingWvp(viewMatrix);

			line.get()->SetVertex(lineVertex);
			line->SetTrandform(lineTransform);
			line.get()->SettingWvp(viewMatrix);

			grid.get()->SettingWvp(viewMatrix);

			sprite->SetTrandform(spriteTransform);
			sprite->SettingWvp();

			sphere->SetTrandform(shpereTransform);
			sphere->SettingWvp(viewMatrix);

			model->SetTransform(objTransform);
			model->SettingWvp(viewMatrix);

			Matrix4x4 uvTransformMatrix = Scale(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Rotation(uvTransformSprite.rotate));
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Translation(uvTransformSprite.translate));
			//spriteMaterial->GetMaterialData()->uvTransform = uvTransformMatrix;
			//ImGuiの内部コマンドを生成
			ImGui::Render();

			engine.get()->LinePreDraw();

			draw.get()->DrawLine(line.get());
			draw.get()->DrawGrid(grid.get());
	
			engine.get()->PreDraw();

			//三角形の描画
			draw->DrawTriangle(triangle.get());
			//スプライトの描画	
			draw.get()->DrawSprite(sprite.get());
			//球の描画
			draw.get()->DrawShpere(sphere.get());
			//objectの描画
			draw->DrawObj(model.get());
			
			engine.get()->PostDraw();

			engine.get()->EndFrame();
		}
	}
	engine.get()->End();

	return 0;
}