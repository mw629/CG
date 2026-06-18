#pragma once
#include <vector>
#include "../MatchaEngine/Core/VariableTypes.h"
#include "HapiColiManager/Recorder.h"

namespace HapiColi {

struct RenderCommand {
    ::Vector3 start;
    ::Vector3 end;
    float color[4];
};

struct SolidRenderCommand {
    enum Type { Sphere, Box };
    Type type;
    ::Vector3 position;
    ::Vector3 scale;
    ::Vector3 rotation;
    float color[4];
};

class HapiColiRenderer {
public:
    void ClearCommands();
    void BuildCommands(const std::vector<ObjectData>& objects);
    const std::vector<RenderCommand>& GetCommands() const;
    const std::vector<SolidRenderCommand>& GetSolidRenderCommands() const;

private:
    std::vector<RenderCommand> commands_;
    std::vector<SolidRenderCommand> solidCommands_;
    
    void AddBoxLines(const ::Vector3& pos, const ::Vector3& scale, const ::Quaternion& rot, float color[4]);
    void AddSphereLines(const ::Vector3& center, float radius, float color[4]);
    void AddSolidSphere(const ::Vector3& center, float radius, float color[4]);
    void AddSolidBox(const ::Vector3& center, const ::Vector3& scale, const ::Vector3& rotation, float color[4]);
};

}
