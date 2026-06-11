#include "JsonSerializer.h"
#include <sstream>

namespace HapiColi
{
    std::string JsonSerializer::EscapeString(const std::string& str)
    {
        std::string escaped = str;
        // Basic escaping (could be expanded if needed)
        size_t pos = 0;
        while ((pos = escaped.find('\"', pos)) != std::string::npos)
        {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        return escaped;
    }

    std::string JsonSerializer::SerializeFrameData(const FrameData& data)
    {
        std::ostringstream oss;
        oss << "{";
        oss << "\"frame\":" << data.frame << ",";
        oss << "\"deltaTime\":" << data.deltaTime << ",";
        oss << "\"objects\":[";
        for (size_t i = 0; i < data.objects.size(); ++i)
        {
            const auto& obj = data.objects[i];
            oss << "{";
            oss << "\"id\":\"" << EscapeString(obj.id) << "\",";
            oss << "\"type\":\"" << EscapeString(obj.type) << "\",";
            oss << "\"position\":[" << obj.position.x << "," << obj.position.y << "," << obj.position.z << "],";
            oss << "\"velocity\":[" << obj.velocity.x << "," << obj.velocity.y << "," << obj.velocity.z << "],";
            oss << "\"collision\":{";
            oss << "\"isColliding\":" << (obj.collision.isColliding ? "true" : "false") << ",";
            oss << "\"collidedWithId\":\"" << EscapeString(obj.collision.collidedWithId) << "\"";
            oss << "}";
            oss << "}";
            if (i < data.objects.size() - 1)
            {
                oss << ",";
            }
        }
        oss << "]";
        oss << "}";
        return oss.str();
    }

    std::string JsonSerializer::SerializeTestResult(const TestResult& result)
    {
        std::ostringstream oss;
        oss << "{";
        oss << "\"frame\":" << result.frame << ",";
        oss << "\"happy\":" << (result.happy ? "true" : "false") << ",";
        oss << "\"expected\":\"" << EscapeString(result.expected) << "\",";
        oss << "\"actual\":\"" << EscapeString(result.actual) << "\",";
        oss << "\"reason\":\"" << EscapeString(result.reason) << "\"";
        oss << "}";
        return oss.str();
    }

    std::string JsonSerializer::SerializeFrames(const std::vector<FrameData>& frames)
    {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < frames.size(); ++i)
        {
            oss << SerializeFrameData(frames[i]);
            if (i < frames.size() - 1)
            {
                oss << ",";
            }
        }
        oss << "]";
        return oss.str();
    }

    // A full JSON parser without external library is complex. 
    // This is a minimal placeholder for the required reading logic.
    bool JsonSerializer::DeserializeFrameData(const std::string& jsonStr, FrameData& outData)
    {
        // For a true engine-independent robust tool, we would integrate a single-header
        // JSON library like picojson or nlohmann json here.
        // For now, we will assume write-only or manual parsing if necessary.
        return false;
    }
}
