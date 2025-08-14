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
	engine.get()->SetBackColor({ 0.1f,0.1f,0.1,0.1f });
	std::unique_ptr<Draw> draw = std::make_unique<Draw>();

	//テクスチャの作成
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int a=texture->CreateTexture("resources/uvChecker.png");
	int b=texture->CreateTexture("resources/nightSky.png");


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
	sprite->Initialize(texture->TextureData(a));
	sprite->CreateSprite();
	Vector2 spritePos[2] = { {0.0f,0.0f}, {300.0,200.0f} };
	Transform spriteTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//球の作成
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
	sphere->Initialize(texture->TextureData(a));
	sphere->CreateSprite();
	Transform shpereTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	//球の作成
	std::unique_ptr<Sphere> sphere2 = std::make_unique<Sphere>();
	sphere2->Initialize(texture->TextureData(a));
	sphere2->CreateSprite();
	Transform shpere2Transform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { -8.0f,0.0f,0.0f } };

	//モデルの作成
	ModelData modelData = LoadObjFile("resources/obj", "axis.obj");
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelData, texture->TextureData(0));
	model->CreateModel();
	Transform objTransform = { {0.5f, 0.5f, 0.5f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,5.0f } };
	//モデルの作成
	ModelData model2Data = LoadObjFile("resources/obj", "axis.obj");
	std::unique_ptr<Model> model2 = std::make_unique<Model>();
	model2->Initialize(model2Data, texture->TextureData(0));
	model2->CreateModel();
	Transform obj2Transform = { {0.5f, 0.5f, 0.5f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f } };
	//Teapot作成
	ModelData teapotData = LoadObjFile("resources/obj", "teapot.obj");
	texture->CreateTexture(teapotData.material.textureDilePath);
	std::unique_ptr<Model> teapot = std::make_unique<Model>();
	teapot->Initialize(teapotData, texture->TextureData(2));
	teapot->CreateModel();
	Transform teapotTransform = { {0.5f, 0.5f, 0.5f}, { 0.0f,3.14f,0.0f }, { 2.0f,0.0f,0.0f } };
	//Bunny作成
	ModelData bunnyData = LoadObjFile("resources/obj", "bunny.obj");
	std::unique_ptr<Model> bunny = std::make_unique<Model>();
	bunny->Initialize(bunnyData, texture->TextureData(0));
	bunny->CreateModel();
	Transform bunnyTransform = { {0.5f, 0.5f, 0.5f}, { 0.0f,3.14f,0.0f }, { 0.0f,0.0f,0.0f } };
	
	//std::unique_ptr<DirectinalLight> directinalLight = std::make_unique<DirectinalLight>();
	//directinalLight->CreateDirectinalLight(graphics->GetDevice());
	std::unique_ptr<DebugCamera> debugCamera = std::make_unique<DebugCamera>();
	debugCamera.get()->Initialize();

	Matrix4x4 viewMatrix;

	bool debugCameraFlag = false;

	bool useMonsterBall = true;

	float move = 8.0f;
	bool trun = false;
	int timer = 0;

	DirectionalLight light = {
	{ 1.0f,1.0f,1.0f,1.0f },
	{ 0.0f, -1.0f, 0.0f },
	{1.0f} };

	Transform camraTransform{ {1.0f,1.0f,1.0f},{0.36f,0.0f,0.0f},{0.0f,10.0f,-25.0f} };

	Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	int BGMHandle;
	audio->Initialize();
	BGMHandle = audio->Load("resources/Audio/BGM/sweet_pop.mp3");
	audio->Play(BGMHandle, true, 1.0f);

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
			timer++;

			gamepad.Update();

			

			engine.get()->NewFrame();

			//ゲーム処理

			gamepad.DrawImGui();
			texture.get()->TextureList();
			engine.get()->ImGuiDraw();

			//開発用UIの処理。実際に開発用UIを出す場合はここをゲ0無固有の処理に置き換える
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::Checkbox("debugCameraFlag", &debugCameraFlag);
			//ImGui::ShowDemoWindow(); debugCameraFlag

			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("camraTranslate", &camraTransform.translate.x, 0.01f);
				ImGui::DragFloat3("camraRotate", &camraTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("camraScale", &camraTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Sphere")) {
				ImGui::DragFloat3("TranslateSphere", &shpereTransform.translate.x, 1.0f);
				ImGui::DragFloat3("RotateSphere", &shpereTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleSphere", &shpereTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("SpriteUV"))
			{
				ImGui::DragFloat2("TranslateSpriteUV", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("ScaleSpriteUV", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("RotateSpriteUV", &uvTransformSprite.rotate.z);
			}

			bunnyTransform.translate.x += gamepad.GetLeftStickX() * 0.1f;
			bunnyTransform.translate.z += gamepad.GetLeftStickY() * 0.1f;
			if (gamepad.PressedButton(XINPUT_GAMEPAD_A)) {	bunnyTransform.translate.y -= 0.1f;}
			if (gamepad.PressedButton(XINPUT_GAMEPAD_B)) {	bunnyTransform.translate.y += 0.1f;}
			bunnyTransform.rotate.y -= gamepad.GetRightStickX() * 0.1f;
			bunnyTransform.rotate.x -= gamepad.GetRightStickY() * 0.1f;



			if (debugCameraFlag) {
				debugCamera->Update(engine.get()->GetInput());
				viewMatrix = debugCamera->GetViewMatrix();
			}
			else
			{
				Matrix4x4 cameraMatrix = MakeAffineMatrix(camraTransform.translate, camraTransform.scale, camraTransform.rotate);;
				viewMatrix = Inverse(cameraMatrix);
			}
			uvTransformSprite.translate.x += 0.01f;
			
			if (move >= 10.0f) {
				trun = true;
			}
			else if (move <= 8.0f) {
				trun = false;
			}
			if(!trun){
				move+=0.1f;
			} else{
				move -= 0.1f;
			}

			shpereTransform.translate.x = move;
			shpereTransform.scale.x = (move-7.0f)*0.5f;
			shpereTransform.rotate.x += 0.1f;
			spriteTransform.translate.x = move;
			
			
		


			if (timer >= 60) {
				sphere2.get()->SetMaterialLighting(true);
				light.direction.x += 1.0f / 120.0f;
				light.direction.y += 1.0f / 120.0f;
			}
			if (light.direction.x >= 1.0f) {
				light.direction.x = 1.0f;
				light.direction.y = 1.0f;
				light.color.x -= 1.0f / 180.0f;
				light.color.z -= 1.0f / 180.0f;
				if (light.color.x <= 0) {
					light.color.x = 1.0;
					light.color.z = 1.0;
					light.direction.x = 0.0f;
					light.direction.y = -1.0f;
					timer = 0;
				}
			}


			engine.get()->SetLight(light);

			triangle->SetVertex(vertex);
			triangle->SetShape();
			triangle->SetTrandform(triangleTransform);
			triangle->SettingWvp(viewMatrix);

			line.get()->SetVertex(lineVertex);
			line->SetTrandform(lineTransform);
			line.get()->SettingWvp(viewMatrix);

			grid.get()->SettingWvp(viewMatrix);

			sprite.get()->SetSize(spritePos[0], spritePos[1]);
			sprite.get()->SetTransfotm(spriteTransform);
			sprite.get()->SetUVTransfotm(uvTransformSprite);
			sprite->SettingWvp();

			sphere->SetTrandform(shpereTransform);
			sphere->SettingWvp(viewMatrix);
			sphere2->SetTrandform(shpere2Transform);
			sphere2->SettingWvp(viewMatrix);

			model->SetTransform(objTransform);
			model->SettingWvp(viewMatrix);
			model2->SetTransform(obj2Transform);
			model2->SettingWvp(viewMatrix);
			bunny->SetTransform(bunnyTransform);
			bunny->SettingWvp(viewMatrix);
			teapot->SetTransform(teapotTransform);
			teapot->SettingWvp(viewMatrix); 


			Matrix4x4 uvTransformMatrix = Scale(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Rotation(uvTransformSprite.rotate));
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Translation(uvTransformSprite.translate));
			//spriteMaterial->GetMaterialData()->uvTransform = uvTransformMatrix;
			//ImGuiの内部コマンドを生成
			ImGui::Render();

			engine.get()->LinePreDraw();

			//draw.get()->DrawLine(line.get());
			//draw.get()->DrawGrid(grid.get());

			engine.get()->PreDraw();

			//スプライトの描画	
			draw.get()->DrawSprite(sprite.get());
			//球の描画
			draw.get()->DrawShpere(sphere.get());
			//球の描画
			draw.get()->DrawShpere(sphere2.get());
			//objectの描画
			draw->DrawObj(model.get());
			draw->DrawObj(model2.get());
			draw->DrawObj(bunny.get());
			draw->DrawObj(teapot.get());

			engine.get()->PostDraw(); 

			engine.get()->EndFrame();
		}
	}
	engine.get()->End();

	return 0;
}