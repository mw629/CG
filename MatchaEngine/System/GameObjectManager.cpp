#include "GameObjectManager.h"
#include <algorithm>
#include <imgui.h>
#include <fstream>
#include <nlohmann/json.hpp>

void GameObjectManager::AddObject(std::shared_ptr<GameObject> obj)
{
    if (obj) {
        objects_.push_back(obj);
    }
}

void GameObjectManager::RemoveObject(std::shared_ptr<GameObject> obj)
{
    auto it = std::remove(objects_.begin(), objects_.end(), obj);
    if (it != objects_.end()) {
        objects_.erase(it, objects_.end());
    }
}

void GameObjectManager::Clear()
{
    objects_.clear();
}

void GameObjectManager::UpdateAll(Matrix4x4 view, float speedMultiplier)
{
    for (auto& obj : objects_) {
        if (obj && obj->GetIsActive()) {
            obj->Update(view, speedMultiplier);
        }
    }
}

void GameObjectManager::DrawAll()
{
    for (auto& obj : objects_) {
        if (obj && obj->GetIsActive()) {
            obj->Draw();
        }
    }
}

void GameObjectManager::ImGuiAll()
{
#ifdef _USE_IMGUI
    if (ImGui::TreeNode("GameObjectManager")) {
        for (auto& obj : objects_) {
            if (obj) {
                if (ImGui::TreeNode(obj->GetName().c_str())) {
                    obj->ImGui();
                    ImGui::TreePop();
                }
            }
        }
        ImGui::TreePop();
    }
#endif
}

void GameObjectManager::SaveScene(const std::string& filepath)
{
    nlohmann::json root;
    nlohmann::json objectsArray = nlohmann::json::array();

    for (const auto& obj : objects_) {
        if (!obj) continue;
        nlohmann::json objJson;
        objJson["name"] = obj->GetName();
        objJson["isActive"] = obj->GetIsActive();
        
        const Transform& t = obj->GetTransform();
        objJson["transform"]["translate"] = { t.translate.x, t.translate.y, t.translate.z };
        objJson["transform"]["rotate"] = { t.rotate.x, t.rotate.y, t.rotate.z };
        objJson["transform"]["scale"] = { t.scale.x, t.scale.y, t.scale.z };
        
        objectsArray.push_back(objJson);
    }
    
    root["objects"] = objectsArray;

    std::ofstream file(filepath);
    if (file.is_open()) {
        file << root.dump(4);
    }
}

void GameObjectManager::LoadScene(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) return;

    nlohmann::json root;
    file >> root;

    if (root.contains("objects")) {
        for (const auto& objJson : root["objects"]) {
            std::string name = objJson["name"];
            for (auto& obj : objects_) {
                if (obj && obj->GetName() == name) {
                    obj->SetIsActive(objJson["isActive"]);
                    
                    if (objJson.contains("transform")) {
                        Transform t;
                        t.translate.x = objJson["transform"]["translate"][0];
                        t.translate.y = objJson["transform"]["translate"][1];
                        t.translate.z = objJson["transform"]["translate"][2];
                        
                        t.rotate.x = objJson["transform"]["rotate"][0];
                        t.rotate.y = objJson["transform"]["rotate"][1];
                        t.rotate.z = objJson["transform"]["rotate"][2];
                        
                        t.scale.x = objJson["transform"]["scale"][0];
                        t.scale.y = objJson["transform"]["scale"][1];
                        t.scale.z = objJson["transform"]["scale"][2];
                        
                        obj->SetTransform(t);
                    }
                    break;
                }
            }
        }
    }
}
