#include "Optimizer.h"
#include <map>
#include <sstream>

namespace HapiColi
{
    std::vector<std::string> Optimizer::GenerateSuggestions(const std::vector<TestResult>& results)
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
            oss << "Detected " << unintendedCollisions << " unintended collisions. Suggestion: Shrink the Collider size.";
            suggestions.push_back(oss.str());
        }

        if (missedCollisions > 0)
        {
            std::ostringstream oss;
            oss << "Detected " << missedCollisions << " missed collisions. Suggestions:";
            oss << "\n- Enlarge the Collider size.";
            oss << "\n- Enable Continuous Collision Detection (CCD).";
            oss << "\n- Use Raycast for high-speed objects.";
            suggestions.push_back(oss.str());
        }

        if (suggestions.empty() && !results.empty())
        {
            suggestions.push_back("No specific optimizations found. Current behavior seems optimal based on rules.");
        }

        return suggestions;
    }
}
