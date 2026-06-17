#include "JsonSerializer.h"
#include <sstream>
#include <unordered_map>

namespace HapiColi
{
    // ---------------------------------------------------------------
    //  ファイルスコープ ヘルパー
    // ---------------------------------------------------------------
    static std::string EscapeStr(const std::string& str)
    {
        std::string escaped = str;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos)
        {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        return escaped;
    }

    static std::string Indent(int depth)
    {
        return std::string(depth * 2, ' ');
    }

    // ---------------------------------------------------------------
    //  ObjectData（インデント付き）
    // ---------------------------------------------------------------
    static std::string SerializeObject(const ObjectData& obj, int depth)
    {
        std::ostringstream oss;
        const std::string i0 = Indent(depth);
        const std::string i1 = Indent(depth + 1);
        const std::string i2 = Indent(depth + 2);

        oss << i0 << "{\n";
        oss << i1 << "\"id\": \""   << EscapeStr(obj.id)   << "\",\n";
        oss << i1 << "\"type\": \"" << EscapeStr(obj.type) << "\",\n";
        oss << i1 << "\"position\": ["
            << obj.position.x << ", "
            << obj.position.y << ", "
            << obj.position.z << "],\n";
        oss << i1 << "\"rotation\": ["
            << obj.rotation.x << ", "
            << obj.rotation.y << ", "
            << obj.rotation.z << ", "
            << obj.rotation.w << "],\n";
        oss << i1 << "\"velocity\": ["
            << obj.velocity.x << ", "
            << obj.velocity.y << ", "
            << obj.velocity.z << "],\n";
            
        oss << i1 << "\"collider\": {\n";
        oss << i2 << "\"type\": " << (int)obj.collider.type << ",\n";
        oss << i2 << "\"size\": ["
            << obj.collider.size.x << ", "
            << obj.collider.size.y << ", "
            << obj.collider.size.z << "]\n";
        oss << i1 << "},\n";
        
        oss << i1 << "\"collision\": {\n";
        oss << i2 << "\"isColliding\": "      << (obj.collision.isColliding ? "true" : "false") << ",\n";
        oss << i2 << "\"collidedWithId\": \"" << EscapeStr(obj.collision.collidedWithId) << "\",\n";
        oss << i2 << "\"hasContactInfo\": "   << (obj.collision.hasContactInfo ? "true" : "false") << ",\n";
        oss << i2 << "\"contactPoint\": ["
            << obj.collision.contactPoint.x << ", "
            << obj.collision.contactPoint.y << ", "
            << obj.collision.contactPoint.z << "],\n";
        oss << i2 << "\"contactNormal\": ["
            << obj.collision.contactNormal.x << ", "
            << obj.collision.contactNormal.y << ", "
            << obj.collision.contactNormal.z << "]\n";
        oss << i1 << "}\n";
        oss << i0 << "}";
        return oss.str();
    }

    // ---------------------------------------------------------------
    //  FrameData（インデント深さ指定）
    // ---------------------------------------------------------------
    static std::string SerializeFrameDataAt(const FrameData& data, int depth)
    {
        std::ostringstream oss;
        const std::string i0 = Indent(depth);
        const std::string i1 = Indent(depth + 1);

        oss << i0 << "{\n";
        oss << i1 << "\"frame\": "     << data.frame     << ",\n";
        oss << i1 << "\"deltaTime\": " << data.deltaTime << ",\n";
        oss << i1 << "\"objects\": [\n";

        for (size_t i = 0; i < data.objects.size(); ++i)
        {
            oss << SerializeObject(data.objects[i], depth + 2);
            if (i < data.objects.size() - 1) oss << ",";
            oss << "\n";
        }

        oss << i1 << "]\n";
        oss << i0 << "}";
        return oss.str();
    }

    // ---------------------------------------------------------------
    //  公開 API
    // ---------------------------------------------------------------

    // private helper – keep signature for header
    std::string JsonSerializer::EscapeString(const std::string& str)
    {
        return EscapeStr(str);
    }

    std::string JsonSerializer::SerializeFrameData(const FrameData& data)
    {
        return SerializeFrameDataAt(data, 0);
    }

    std::string JsonSerializer::SerializeTestResult(const TestResult& result)
    {
        std::ostringstream oss;
        oss << "{\n";
        oss << "    \"frame\": "      << result.frame << ",\n";
        oss << "    \"happy\": "      << (result.happy ? "true" : "false") << ",\n";
        oss << "    \"expected\": \"" << EscapeStr(result.expected) << "\",\n";
        oss << "    \"actual\": \""   << EscapeStr(result.actual)   << "\",\n";
        oss << "    \"reason\": \""   << EscapeStr(result.reason)   << "\"\n";
        oss << "  }";
        return oss.str();
    }

    std::string JsonSerializer::SerializeFrames(const std::vector<FrameData>& frames)
    {
        std::ostringstream oss;
        oss << "[\n";
        for (size_t i = 0; i < frames.size(); ++i)
        {
            oss << SerializeFrameDataAt(frames[i], 1);
            if (i < frames.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "]\n";
        return oss.str();
    }

    std::string JsonSerializer::SerializeCollisionSummary(const std::vector<FrameData>& frames)
    {
        struct CollisionPeriod {
            std::string idA;
            std::string idB;
            uint32_t start_frame;
            uint32_t end_frame;
        };
        std::vector<CollisionPeriod> periods;
        struct ActiveCol { std::string idA; std::string idB; uint32_t start; };
        std::vector<ActiveCol> activeList;

        for (const auto& frame : frames) {
            std::vector<std::pair<std::string, std::string>> currentCols;
            for (const auto& obj : frame.objects) {
                if (obj.collision.isColliding && !obj.collision.collidedWithId.empty()) {
                    std::string a = obj.id;
                    std::string b = obj.collision.collidedWithId;
                    if (a > b) std::swap(a, b);
                    bool found = false;
                    for (const auto& p : currentCols) {
                        if (p.first == a && p.second == b) { found = true; break; }
                    }
                    if (!found) currentCols.push_back({a, b});
                }
            }

            for (auto it = activeList.begin(); it != activeList.end(); ) {
                bool stillColliding = false;
                for (const auto& cc : currentCols) {
                    if (cc.first == it->idA && cc.second == it->idB) {
                        stillColliding = true; break;
                    }
                }
                if (!stillColliding) {
                    periods.push_back({it->idA, it->idB, it->start, frame.frame - 1});
                    it = activeList.erase(it);
                } else {
                    ++it;
                }
            }

            for (const auto& cc : currentCols) {
                bool isNew = true;
                for (const auto& ac : activeList) {
                    if (ac.idA == cc.first && ac.idB == cc.second) {
                        isNew = false; break;
                    }
                }
                if (isNew) {
                    activeList.push_back({cc.first, cc.second, frame.frame});
                }
            }
        }

        if (!frames.empty()) {
            uint32_t lastFrame = frames.back().frame;
            for (const auto& ac : activeList) {
                periods.push_back({ac.idA, ac.idB, ac.start, lastFrame});
            }
        }

        std::ostringstream oss;
        oss << "{\n  \"collision_periods\": [\n";
        for (size_t i = 0; i < periods.size(); ++i) {
            oss << "    {\n";
            oss << "      \"idA\": \"" << EscapeStr(periods[i].idA) << "\",\n";
            oss << "      \"idB\": \"" << EscapeStr(periods[i].idB) << "\",\n";
            oss << "      \"start_frame\": " << periods[i].start_frame << ",\n";
            oss << "      \"end_frame\": " << periods[i].end_frame << "\n";
            oss << "    }";
            if (i < periods.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "  ]\n}\n";
        return oss.str();
    }

    std::string JsonSerializer::SerializeUnhappyReport(
        const std::vector<TestResult>& results,
        const std::vector<FrameData>& frames)
    {
        std::unordered_map<uint32_t, const FrameData*> frameMap;
        for (const auto& f : frames)
        {
            if (frameMap.find(f.frame) == frameMap.end())
                frameMap[f.frame] = &f;
        }

        // 集計
        int totalTests  = (int)results.size();
        int unhappyCount = 0;
        for (const auto& r : results)
            if (!r.happy) ++unhappyCount;
        int happyCount = totalTests - unhappyCount;
        float successRate = (totalTests > 0) ? (float)happyCount / totalTests * 100.0f : 0.0f;

        std::ostringstream oss;
        oss << "{\n";

        // ── 結論（サマリー）を先頭に ──────────────────────
        oss << "  \"summary\": {\n";
        oss << "    \"total_tests\": "   << totalTests   << ",\n";
        oss << "    \"happy\": "         << happyCount   << ",\n";
        oss << "    \"unhappy\": "       << unhappyCount << ",\n";

        // 小数点1桁で出力
        char rateBuf[32];
        snprintf(rateBuf, sizeof(rateBuf), "%.1f", successRate);
        oss << "    \"success_rate\": \"" << rateBuf << "%\"\n";
        oss << "  },\n";

        // ── 失敗詳細 ──────────────────────────────────────
        oss << "  \"unhappy_report\": [\n";

        bool first = true;
        for (const auto& result : results)
        {
            if (result.happy) continue;

            if (!first) oss << ",\n";
            first = false;

            oss << "  {\n";
            oss << "    \"result\": " << SerializeTestResult(result) << ",\n";

            auto it = frameMap.find(result.frame);
            if (it != frameMap.end())
            {
                oss << "    \"frame_data\":\n";
                oss << SerializeFrameDataAt(*it->second, 2) << "\n";
            }
            else
            {
                oss << "    \"frame_data\": null\n";
            }

            oss << "  }";
        }

        oss << "\n  ]\n}\n";
        return oss.str();
    }

    bool JsonSerializer::DeserializeFrameData(const std::string& jsonStr, FrameData& outData)
    {
        return false;
    }
}
