#include "JsonSerializer.h"
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <utility>

namespace HapiColi
{
    // ---------------------------------------------------------------
    //  File scope helper
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
    //  ObjectData (With indent)
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
    //  FrameData (Indent depth)
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
    //  Public API
    // ---------------------------------------------------------------

    // private helper - keep signature for header
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

    std::string JsonSerializer::SerializeMarkdownReport(
        const std::vector<TestResult>& results,
        const std::vector<FrameData>& frames,
        const std::vector<FuzzResult>& fuzzResults,
        Language language)
    {
        auto GetText = [language](const char* en, auto jp) {
            return language == Language::Japanese ? (const char*)jp : en;
        };

        // 1. Collision Summary Calculation
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

        // 2. Unhappy Report Calculation
        std::unordered_map<uint32_t, const FrameData*> frameMap;
        for (const auto& f : frames)
        {
            if (frameMap.find(f.frame) == frameMap.end())
                frameMap[f.frame] = &f;
        }

        int totalTests  = (int)results.size();
        int unhappyCount = 0;
        for (const auto& r : results)
            if (!r.happy) ++unhappyCount;
        int happyCount = totalTests - unhappyCount;
        float successRate = (totalTests > 0) ? (float)happyCount / totalTests * 100.0f : 0.0f;

        // 3. Serialize to Markdown
        std::ostringstream oss;
        oss << GetText("# HapiColi Test Report\n\n", u8"# HapiColi テストレポート\n\n");

        // Summary
        oss << GetText("## 1. Summary\n", u8"## 1. サマリー\n");
        oss << GetText("- **Total Tests:** ", u8"- **総テスト数:** ") << totalTests << "\n";
        oss << GetText("- **Happy (Success):** ", u8"- **成功 (Happy):** ") << happyCount << "\n";
        oss << GetText("- **Unhappy (Failed):** ", u8"- **失敗 (Unhappy):** ") << unhappyCount << "\n";
        char rateBuf[32];
        snprintf(rateBuf, sizeof(rateBuf), "%.1f", successRate);
        oss << GetText("- **Success Rate:** ", u8"- **成功率:** ") << rateBuf << "%\n\n";

        // Collision Periods
        oss << GetText("## 2. Collision Periods\n", u8"## 2. 衝突期間 (Collision Periods)\n");
        if (periods.empty()) {
            oss << GetText("No collisions detected.\n\n", u8"衝突は検知されませんでした。\n\n");
        } else {
            oss << GetText("| Object A | Object B | Start Frame | End Frame |\n", u8"| オブジェクト A | オブジェクト B | 開始フレーム | 終了フレーム |\n");
            oss << "|---|---|---|---|\n";
            for (const auto& p : periods) {
                oss << "| " << p.idA << " | " << p.idB << " | " << p.start_frame << " | " << p.end_frame << " |\n";
            }
            oss << "\n";
        }

        // Test details (Both Happy and Unhappy)
        oss << GetText("## 3. Test Details\n", u8"## 3. テストの詳細\n");
        if (results.empty()) {
            oss << GetText("No tests recorded.\n\n", u8"記録されたテストはありません。\n\n");
        } else {
            oss << GetText("| Frame | Status | Expected | Actual | Subject Pos | Target Pos | Distance | Reason |\n", 
                           u8"| フレーム | 状態 | 期待値 | 実際 | 主語座標 | 対象座標 | 距離 | 理由 |\n");
            oss << "|---|---|---|---|---|---|---|---|\n";
            for (const auto& result : results)
            {
                std::string statusStr = result.happy ? GetText("Happy", u8"成功") : GetText("Unhappy", u8"失敗");
                std::string reasonStr = result.reason;
                if (language == Language::Japanese) {
                    if (reasonStr == "Success") reasonStr = (const char*)u8"成功";
                    else if (reasonStr == "Failed to hit target") reasonStr = (const char*)u8"ターゲットへの衝突失敗";
                    else if (reasonStr == "Unintended collision") reasonStr = (const char*)u8"意図しない衝突";
                }

                std::string targetName = result.expected;
                if (targetName.find("Hit ") == 0) targetName = targetName.substr(4);
                else if (targetName.find("No Hit ") == 0) targetName = targetName.substr(7);

                std::string expectedStr = result.expected;
                std::string actualStr = result.actual;
                if (language == Language::Japanese) {
                    if (expectedStr.find("Hit ") == 0) expectedStr = std::string((const char*)u8"衝突: ") + expectedStr.substr(4);
                    else if (expectedStr.find("No Hit ") == 0) expectedStr = std::string((const char*)u8"非衝突: ") + expectedStr.substr(7);

                    if (actualStr.find("Hit ") == 0) actualStr = std::string((const char*)u8"衝突: ") + actualStr.substr(4);
                    else if (actualStr == "No Hit") actualStr = (const char*)u8"非衝突";
                    else if (actualStr.find("Did not hit ") == 0) actualStr = std::string((const char*)u8"非衝突: ") + actualStr.substr(12);
                }

                std::string subPosStr = "N/A";
                std::string tarPosStr = "N/A";
                std::string distStr = "N/A";

                auto it = frameMap.find(result.frame);
                if (it != frameMap.end())
                {
                    const FrameData* f = it->second;
                    const ObjectData* subObj = f->GetObjectById(result.targetId); // result.targetId is subject
                    const ObjectData* tarObj = f->GetObjectById(targetName);

                    if (subObj) {
                        char buf[64];
                        snprintf(buf, sizeof(buf), "(%.2f, %.2f, %.2f)", subObj->position.x, subObj->position.y, subObj->position.z);
                        subPosStr = result.targetId + " " + buf;
                    }
                    if (tarObj) {
                        char buf[64];
                        snprintf(buf, sizeof(buf), "(%.2f, %.2f, %.2f)", tarObj->position.x, tarObj->position.y, tarObj->position.z);
                        tarPosStr = targetName + " " + buf;
                    }
                    if (subObj && tarObj) {
                        float dx = subObj->position.x - tarObj->position.x;
                        float dy = subObj->position.y - tarObj->position.y;
                        float dz = subObj->position.z - tarObj->position.z;
                        float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
                        char buf[32];
                        snprintf(buf, sizeof(buf), "%.2f", dist);
                        distStr = buf;
                    }
                }

                oss << "| " << result.frame 
                    << " | " << statusStr 
                    << " | " << expectedStr 
                    << " | " << actualStr 
                    << " | " << subPosStr 
                    << " | " << tarPosStr 
                    << " | " << distStr 
                    << " | " << reasonStr << " |\n";
            }
            oss << "\n";
        }

        // Fuzzing Results
        oss << GetText("## 4. Fuzzing Results\n", u8"## 4. ファジング結果 (Fuzzing Results)\n");
        if (fuzzResults.empty()) {
            oss << GetText("No fuzzing tests recorded.\n\n", u8"記録されたファジングテストはありません。\n\n");
        } else {
            oss << GetText("| Test Name | Status | Object A Pos | Object B Pos | Description |\n", 
                           u8"| テスト名 | 状態 | オブジェクトA 座標 | オブジェクトB 座標 | 説明 |\n");
            oss << "|---|---|---|---|---|\n";
            for (const auto& f : fuzzResults) {
                std::string statusStr = f.isBugCaught ? GetText("Bug Caught", u8"バグ検知") : GetText("Passed", u8"パス");
                
                char bufA[64];
                snprintf(bufA, sizeof(bufA), "(%.2f, %.2f, %.2f)", f.objA.position.x, f.objA.position.y, f.objA.position.z);
                
                char bufB[64];
                snprintf(bufB, sizeof(bufB), "(%.2f, %.2f, %.2f)", f.objB.position.x, f.objB.position.y, f.objB.position.z);

                std::string descStr = f.description;
                if (language == Language::Japanese) {
                    if (descStr == "Unexpected collision detected!") descStr = (const char*)u8"予期せぬ衝突を検知！";
                    else if (descStr == "Missed expected collision!") descStr = (const char*)u8"想定された衝突をミス！";
                    else if (descStr == "OK") descStr = (const char*)u8"正常";
                }

                oss << "| " << f.testName
                    << " | " << statusStr
                    << " | " << f.objA.id << " " << bufA
                    << " | " << f.objB.id << " " << bufB
                    << " | " << descStr << " |\n";
            }
            oss << "\n";
        }

        return oss.str();
    }

    bool JsonSerializer::DeserializeFrameData(const std::string& jsonStr, FrameData& outData)
    {
        return false;
    }
}
