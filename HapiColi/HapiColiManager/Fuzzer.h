#pragma once
#include <string>
#include <vector>
#include <functional>
#include "HapiColiTypes.h"

namespace HapiColi {

    struct FuzzResult {
        std::string testName;
        ObjectData objA;
        ObjectData objB;
        CollisionResult collisionA;
        CollisionResult collisionB;
        bool isBugCaught; // True if an unexpected result was found
        std::string description;
    };

    struct FuzzerConfig {
        bool enableMicroOffsetTest = true;
        float microOffsetRange = 0.01f;
        int microOffsetTrials = 50;
    };

    class Fuzzer {
    public:
        // The callback used to run collision detection logic from the game engine
        using CollisionFunc = std::function<void(ObjectData& a, ObjectData& b)>;

        struct FuzzTarget {
            std::string name;
            ObjectData baseA;
            ObjectData baseB;
            CollisionFunc evalFunc;
        };

        void RegisterTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB, CollisionFunc func);
        void UpdateTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB);
        void RunAll();
        
        const std::vector<FuzzResult>& GetResults() const { return m_results; }
        void ClearResults() { m_results.clear(); }

        FuzzerConfig& GetConfig() { return m_config; }

    private:
        FuzzerConfig m_config;
        std::vector<FuzzTarget> m_targets;
        std::vector<FuzzResult> m_results;

        void RunVariations(const FuzzTarget& target);
    };

}
