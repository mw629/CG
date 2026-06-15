#pragma once
#include <memory>
#include "System/GameObjectManager.h"

class EditorUI {
private:
    std::shared_ptr<GameObject> selectedObject_ = nullptr;

public:
    void Draw(GameObjectManager* gameObjectManager);
    
    std::shared_ptr<GameObject> GetSelectedObject() const { return selectedObject_; }
    void SetSelectedObject(std::shared_ptr<GameObject> obj) { selectedObject_ = obj; }
};
