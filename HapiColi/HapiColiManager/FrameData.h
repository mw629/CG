#pragma once

#include "HapiColiTypes.h"

namespace HapiColi
{
    struct FrameData
    {
        uint32_t frame = 0;
        float deltaTime = 0.0f;
        std::vector<ObjectData> objects;

        // Helper to find an object by ID
        const ObjectData* GetObjectById(const std::string& id) const
        {
            for (const auto& obj : objects)
            {
                if (obj.id == id)
                {
                    return &obj;
                }
            }
            return nullptr;
        }
    };
}
