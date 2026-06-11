#include "Optimizer.h"
#include <map>
#include <sstream>

namespace HapiColi
{
    std::vector<std::string> Optimizer::GenerateSuggestions(const std::vector<TestResult>& results, Language lang)
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

        if (suggestions.empty() && !results.empty())
        {
            if (lang == Language::Japanese)
                suggestions.push_back((const char*)u8"特に改善案は見つかりませんでした。ルールに基づくと現在の挙動は最適です。");
            else
                suggestions.push_back("No specific optimizations found. Current behavior seems optimal based on rules.");
        }

        return suggestions;
    }
}
