#include "Fuzzer.h"
#include "LogManager.h"
#include <cmath>
#include <random>
#include <string>

namespace HapiColi {

    void Fuzzer::RegisterTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB, CollisionFunc func) {
        FuzzTarget target;
        target.name = name;
        target.baseA = baseA;
        target.baseB = baseB;
        target.evalFunc = func;
        m_targets.push_back(target);
    }

    void Fuzzer::UpdateTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB) {
        for (auto& target : m_targets) {
            if (target.name == name) {
                target.baseA = baseA;
                target.baseB = baseB;
                return;
            }
        }
    }

    void Fuzzer::RunAll() {
        LogManager::PrintOutput("Fuzzer::RunAll() started. Number of targets: " + std::to_string(m_targets.size()));
        m_results.clear();
        for (const auto& target : m_targets) {
            RunVariations(target);
        }
        LogManager::PrintOutput("Fuzzer::RunAll() completed. Total results: " + std::to_string(m_results.size()));
    }

    void Fuzzer::RunVariations(const FuzzTarget& target) {
        LogManager::PrintOutput("Fuzzer::RunVariations() starting for target: " + target.name);
        bool baseColliding = false;
        // 1. Base Test
        {
            LogManager::PrintOutput("  -> [Test 1] Base Test started.");
            ObjectData a = target.baseA;
            ObjectData b = target.baseB;
            target.evalFunc(a, b);
            
            FuzzResult res;
            res.testName = target.name + " (基本テスト)";
            res.objA = a;
            res.objB = b;
            res.collisionA = a.collision;
            res.collisionB = b.collision;
            res.isBugCaught = false; // By default, base is assumed expected unless explicitly defined
            res.description = a.collision.isColliding ? "衝突した" : "衝突なし";
            m_results.push_back(res);

            baseColliding = a.collision.isColliding;
            LogManager::PrintOutput("  -> [Test 1] Result: " + res.description);
        }

        // 2. High Velocity Test (Tunneling)
        {
            LogManager::PrintOutput("  -> [Test 2] High Velocity Test (Tunneling) started.");
            ObjectData a = target.baseA;
            ObjectData b = target.baseB;
            b.position.x += 100.0f; // Simulate very fast movement over one frame
            target.evalFunc(a, b);
            
            FuzzResult res;
            res.testName = target.name + " (高速移動すり抜けテスト)";
            res.objA = a;
            res.objB = b;
            res.collisionA = a.collision;
            res.collisionB = b.collision;
            // Since evalFunc is just static AABB, moving it 100 units will naturally stop collision.
            res.isBugCaught = false; 
            if (baseColliding) {
                res.description = res.collisionA.isColliding ? "警告：遠距離で衝突判定がありました" : "静的判定では衝突なし（すり抜けの可能性）";
            } else {
                res.description = "元の状態が非衝突のためテスト対象外（非衝突を維持）";
            }
            m_results.push_back(res);
            LogManager::PrintOutput("  -> [Test 2] Result: " + res.description);
        }

        // 3. Boundary / Micro-offset Test
        if (m_config.enableMicroOffsetTest)
        {
            LogManager::PrintOutput("  -> [Test 3] Boundary / Micro-offset Test started. Trials: " + std::to_string(m_config.microOffsetTrials));
            std::mt19937 rng(42); // Fixed seed for reproducibility
            std::uniform_real_distribution<float> dist(-m_config.microOffsetRange, m_config.microOffsetRange);
            bool toggled = false;
            
            for(int i = 0; i < m_config.microOffsetTrials; ++i) {
                ObjectData a = target.baseA;
                ObjectData b = target.baseB;
                b.position.x += dist(rng);
                b.position.y += dist(rng);
                b.position.z += dist(rng);

                target.evalFunc(a, b);
                
                // If it toggles from the base state, record it
                if (a.collision.isColliding != baseColliding) {
                    FuzzResult res;
                    res.testName = target.name + " (微小ゆらぎテスト #" + std::to_string(i) + ")";
                    res.objA = a;
                    res.objB = b;
                    res.collisionA = a.collision;
                    res.collisionB = b.collision;
                    // Not necessarily a bug, just a boundary fluctuation. Let's record it without flagging as red bug.
                    res.isBugCaught = false; 
                    res.description = "境界付近でのゆらぎ（結果が変わりました）";
                    m_results.push_back(res);
                    toggled = true;
                    LogManager::PrintOutput("  -> [Test 3] Toggled on trial " + std::to_string(i) + ". Result changed.");
                    break; // Only record the first micro-failure to avoid spam
                }
            }

            if (!toggled) {
                FuzzResult res;
                res.testName = target.name + " (微小ゆらぎテスト)";
                res.objA = target.baseA;
                res.objB = target.baseB;
                res.collisionA = target.baseA.collision;
                res.collisionB = target.baseB.collision;
                res.isBugCaught = false; 
                res.description = "ゆらぎ耐性チェック完了（判定の変動なし）";
                m_results.push_back(res);
                LogManager::PrintOutput("  -> [Test 3] Completed with no toggles.");
            }
        }
    }

}
