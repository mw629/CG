#include "EditorUI.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif

void EditorUI::Draw(GameObjectManager* gameObjectManager)
{
#ifdef _USE_IMGUI
    if (!gameObjectManager) return;

    // Hierarchy Window
    ImGui::Begin("Hierarchy");
    for (auto& obj : gameObjectManager->GetObjects()) {
        if (!obj) continue;
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
        if (selectedObject_ == obj) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        // 非アクティブなオブジェクトはグレーで表示（Unity風）
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
        // Name
        ImGui::Text("Name: %s", selectedObject_->GetName().c_str());
        ImGui::Separator();

        // Active Checkbox
        bool isActive = selectedObject_->GetIsActive();
        if (ImGui::Checkbox("Active", &isActive)) {
            selectedObject_->SetIsActive(isActive);
        }
        ImGui::Separator();

        // Specific ImGui (Transform, Shader, Blend, Material, etc.)
        selectedObject_->ImGui();
    } else {
        ImGui::Text("No object selected.");
    }
    ImGui::End();
#endif
}
