#include "Optimizer.h"
#include <map>
#include <sstream>

namespace HapiColi
{
    std::vector<std::string> Optimizer::GenerateSuggestions(
        const std::vector<TestResult>& results,
        const std::vector<AnalyzerWarning>& warnings,
        const std::vector<FuzzResult>& fuzzResults,
        Language lang)
    {
        std::vector<std::string> suggestions;
        
        int unintendedCollisions = 0;
        int missedCollisions = 0;

        for (const auto& result : results)
        {
            if (!result.happy)
            {
                if (result.reason == "Unintended collision")
                {
                    unintendedCollisions++;
                }
                else if (result.reason == "Failed to hit target")
                {
                    missedCollisions++;
                }
            }
        }

        if (unintendedCollisions > 0)
        {
            std::ostringstream oss;
            if (lang == Language::Japanese)
                oss << unintendedCollisions << (const char*)u8"回の意図しない衝突が検知されました。提案: コライダーサイズを縮小してください。";
            else
                oss << "Detected " << unintendedCollisions << " unintended collisions. Suggestion: Shrink the Collider size.";
            suggestions.push_back(oss.str());
        }

        if (missedCollisions > 0)
        {
            std::ostringstream oss;
            if (lang == Language::Japanese)
            {
                oss << missedCollisions << (const char*)u8"回の判定抜けが検知されました。提案:\n";
                oss << (const char*)u8"- コライダーサイズを拡大してください。\n";
                oss << (const char*)u8"- 連続衝突検知(CCD)を有効にしてください。\n";
                oss << (const char*)u8"- 高速な物体にはレイキャストを使用してください。";
            }
            else
            {
                oss << "Detected " << missedCollisions << " missed collisions. Suggestions:\n";
                oss << "- Enlarge the Collider size.\n";
                oss << "- Enable Continuous Collision Detection (CCD).\n";
                oss << "- Use Raycast for high-speed objects.";
            }
            suggestions.push_back(oss.str());
        }

        // Warning Analysis
        int jitterCount = 0;
        int highSpeedCount = 0;
        for (const auto& w : warnings) {
            if (w.message.find("Jitter") != std::string::npos || w.message.find((const char*)u8"ガタつき") != std::string::npos) {
                jitterCount++;
            } else if (w.message.find((const char*)u8"高速衝突") != std::string::npos) {
                highSpeedCount++;
            }
        }

        if (jitterCount > 0) {
            std::ostringstream oss;
            if (lang == Language::Japanese) {
                oss << (const char*)u8"ガタつき（Jitter）が " << jitterCount << (const char*)u8" 件検知されました。提案:\n";
                oss << (const char*)u8"- 物理マテリアルの反発係数（Bounciness）を下げる。\n";
                oss << (const char*)u8"- 押し出し処理（Penetration Resolution）にヒステリシスを持たせる。";
            } else {
                oss << "Jitter detected " << jitterCount << " times. Suggestions:\n";
                oss << "- Lower the Bounciness of physics materials.\n";
                oss << "- Add hysteresis to Penetration Resolution.";
            }
            suggestions.push_back(oss.str());
        }

        if (highSpeedCount > 0 && missedCollisions == 0) { // If missedCollisions > 0, it's already suggested
            std::ostringstream oss;
            if (lang == Language::Japanese) {
                oss << (const char*)u8"すり抜けリスクのある高速移動が " << highSpeedCount << (const char*)u8" 件検知されました。提案:\n";
                oss << (const char*)u8"- 連続衝突検知(CCD)を有効にしてください。\n";
                oss << (const char*)u8"- 高速な物体にはレイキャストを使用してください。";
            } else {
                oss << "High-speed movement with tunneling risk detected " << highSpeedCount << " times. Suggestions:\n";
                oss << "- Enable Continuous Collision Detection (CCD).\n";
                oss << "- Use Raycast for high-speed objects.";
            }
            suggestions.push_back(oss.str());
        }

        // Fuzzing Analysis
        int fuzzBugCount = 0;
        for (const auto& f : fuzzResults) {
            if (f.isBugCaught) fuzzBugCount++;
        }
        if (fuzzBugCount > 0) {
            std::ostringstream oss;
            if (lang == Language::Japanese) {
                oss << (const char*)u8"ファジングテストで " << fuzzBugCount << (const char*)u8" 件の問題が検出されました。提案:\n";
                oss << (const char*)u8"- ゆらぎで判定が変わるため、境界値付近での判定マージン（Skin Widthなど）を見直してください。";
            } else {
                oss << "Fuzzing tests found " << fuzzBugCount << " issues. Suggestion:\n";
                oss << "- Review collision margins (e.g., Skin Width) near boundary values.";
            }
            suggestions.push_back(oss.str());
        }

        if (suggestions.empty() && (!results.empty() || !warnings.empty() || !fuzzResults.empty()))
        {
            if (lang == Language::Japanese)
                suggestions.push_back((const char*)u8"特に改善案は見つかりませんでした。ルールに基づくと現在の挙動は最適です。");
            else
                suggestions.push_back("No specific optimizations found. Current behavior seems optimal based on rules.");
        }

        return suggestions;
    }
}
