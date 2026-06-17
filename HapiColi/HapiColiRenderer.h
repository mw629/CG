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

class HapiColiRenderer {
public:
    void ClearCommands();
    void BuildCommands(const std::vector<ObjectData>& objects);
    const std::vector<RenderCommand>& GetCommands() const;

private:
    std::vector<RenderCommand> commands_;
    
    void AddBoxLines(const ::Vector3& pos, const ::Vector3& scale, const ::Quaternion& rot, float color[4]);
};

}
