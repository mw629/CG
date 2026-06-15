#include "EditorUI.h"
#include "../MatchaEngine/Input/Input.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#include "ImGuizmo.h"
#endif

void EditorUI::ProcessMousePicking(GameObjectManager* gameObjectManager, const Matrix4x4& view, const Matrix4x4& projection) {
#ifdef _USE_IMGUI
    if (!gameObjectManager) return;

    // Sceneウィンドウの矩形とホバー状態を取得
    ImVec2 vMin, vMax, windowPos;
    bool isSceneHovered = false;
    ImGui::Begin("Scene");
    vMin = ImGui::GetWindowContentRegionMin();
    vMax = ImGui::GetWindowContentRegionMax();
    windowPos = ImGui::GetWindowPos();
    isSceneHovered = ImGui::IsWindowHovered();
    ImGui::End();

    // Sceneウィンドウにホバー中でない・ギズモ操作中は無視
    if (!isSceneHovered) return;
    if (ImGuizmo::IsOver()) return;

    // 左クリックの瞬間だけ処理
    if (!Input::PushMouse(0)) return;

    // ImGuiからマウス座標を取得
    ImVec2 mousePos = ImGui::GetMousePos();
    float mx = mousePos.x;
    float my = mousePos.y;

    // Sceneウィンドウの屋根左山の絶対座標とサイズ
    ImVec2 scenePos = ImVec2(vMin.x + windowPos.x, vMin.y + windowPos.y);
    ImVec2 sceneSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

    // NDC変換 (Sceneウィンドウ内を-1〜1にマッピング)
    float ndcX = (2.0f * (mx - scenePos.x)) / sceneSize.x - 1.0f;
    float ndcY = 1.0f - (2.0f * (my - scenePos.y)) / sceneSize.y;

    // Sceneウィンドウ外をクリックした場合は無視
    if (ndcX < -1.0f || ndcX > 1.0f || ndcY < -1.0f || ndcY > 1.0f) return;

    // ニア・ファークリップ坐標でレイを生成
    Vector3 nearPoint = { ndcX, ndcY, 0.0f };
    Vector3 farPoint  = { ndcX, ndcY, 1.0f };

    Matrix4x4 viewProj    = MultiplyMatrix4x4(view, projection);
    Matrix4x4 invViewProj = Inverse(viewProj);

    Vector3 rayStart = TransformMatrix(nearPoint, invViewProj);
    Vector3 rayEnd   = TransformMatrix(farPoint,  invViewProj);

    Ray ray;
    ray.origin    = rayStart;
    ray.direction = Normalize(rayEnd - rayStart);

    float closestDist = FLT_MAX;
    std::shared_ptr<GameObject> closestObj = nullptr;

    for (auto& obj : gameObjectManager->GetObjects()) {
        if (!obj->GetIsActive()) continue;

        AABB aabb = GetAABB(obj->GetTransform(), 2.0f, 2.0f);

        float dist = 0.0f;
        if (CheckRayAABB(ray, aabb, dist)) {
            if (dist < closestDist) {
                closestDist = dist;
                closestObj  = obj;
            }
        }
    }

    if (closestObj) {
        selectedObject_ = closestObj;
    }
#endif
}

void EditorUI::DrawGizmo(const Matrix4x4& view, const Matrix4x4& projection) {
#ifdef _USE_IMGUI
    if (!selectedObject_) return;

    // 現在アクティブなウィンドウのContentRegionの座標を取得
    // (SceneウィンドウのBegin/Endの間から呼ばれることを前提とする)
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    ImVec2 windowPos = ImGui::GetWindowPos();

    ImVec2 scenePos = ImVec2(vMin.x + windowPos.x, vMin.y + windowPos.y);
    ImVec2 sceneSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(scenePos.x, scenePos.y, sceneSize.x, sceneSize.y);

    Transform t = selectedObject_->GetTransform();
    Matrix4x4 world = MakeAffineMatrix(t.translate, t.scale, t.rotate);

    ImGuizmo::Manipulate(
        &view.m[0][0],
        &projection.m[0][0],
        ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE,
        ImGuizmo::LOCAL,
        &world.m[0][0]
    );

    if (ImGuizmo::IsUsing()) {
        // 分解してTransformに戻す
        float translation[3], rotation[3], scale[3];
        ImGuizmo::DecomposeMatrixToComponents(&world.m[0][0], translation, rotation, scale);

        // ラジアンへ変換
        float pi = 3.1415926535f;
        t.translate = { translation[0], translation[1], translation[2] };
        t.rotate = { rotation[0] * pi / 180.0f, rotation[1] * pi / 180.0f, rotation[2] * pi / 180.0f };
        t.scale = { scale[0], scale[1], scale[2] };

        selectedObject_->SetTransform(t);
    }
#endif
}

void EditorUI::Draw(GameObjectManager* gameObjectManager, const Matrix4x4& view, const Matrix4x4& projection)
{
#ifdef _USE_IMGUI
    if (!gameObjectManager) return;

    // マウスピッキング
    ProcessMousePicking(gameObjectManager, view, projection);
    // (ギズモはEngineのSceneウィンドウコールバックから描画される)

    // Hierarchy Window
    ImGui::Begin("Hierarchy");
    for (auto& obj : gameObjectManager->GetObjects()) {
        if (!obj) continue;
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
        if (selectedObject_ == obj) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (!obj->GetIsActive()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        bool isOpen = ImGui::TreeNodeEx((void*)obj.get(), flags, "%s", obj->GetName().c_str());
        
        if (!obj->GetIsActive()) {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemClicked()) {
            selectedObject_ = obj;
        }

        if (isOpen) {
            ImGui::TreePop();
        }
    }
    ImGui::End();

    // Inspector Window
    ImGui::Begin("Inspector");
    if (selectedObject_) {
        ImGui::Text("Name: %s", selectedObject_->GetName().c_str());
        ImGui::Separator();

        bool isActive = selectedObject_->GetIsActive();
        if (ImGui::Checkbox("Active", &isActive)) {
            selectedObject_->SetIsActive(isActive);
        }
        ImGui::Separator();

        selectedObject_->ImGui();
    } else {
        ImGui::Text("No object selected.");
    }
    ImGui::End();
#endif
}
