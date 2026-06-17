#include "Fuzzer.h"
#include <cmath>
#include <random>

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
        m_results.clear();
        for (const auto& target : m_targets) {
            RunVariations(target);
        }
    }

    void Fuzzer::RunVariations(const FuzzTarget& target) {
        // 1. Base Test
        {
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
        }

        // 2. High Velocity Test (Tunneling)
        {
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
            // If it's a known tunnel setup and it fails to collide, that's a bug!
            // But generically, we just log it. If it doesn't collide, we flag it as a potential tunneling bug.
            res.isBugCaught = !a.collision.isColliding; 
            res.description = res.isBugCaught ? "バグ：すり抜けが発生しました！" : "すり抜け防止成功（衝突しました）";
            m_results.push_back(res);
        }

        // 3. Boundary / Micro-offset Test
        {
            std::mt19937 rng(42); // Fixed seed for reproducibility
            std::uniform_real_distribution<float> dist(-0.01f, 0.01f);
            
            for(int i = 0; i < 50; ++i) {
                ObjectData a = target.baseA;
                ObjectData b = target.baseB;
                b.position.x += dist(rng);
                b.position.y += dist(rng);
                b.position.z += dist(rng);

                target.evalFunc(a, b);
                
                // If it toggles in a way we don't expect, record it
                if (!a.collision.isColliding) {
                    FuzzResult res;
                    res.testName = target.name + " (微小ゆらぎテスト #" + std::to_string(i) + ")";
                    res.objA = a;
                    res.objB = b;
                    res.collisionA = a.collision;
                    res.collisionB = b.collision;
                    res.isBugCaught = true; 
                    res.description = "バグ：境界線の精度エラー（すり抜け発生）！";
                    m_results.push_back(res);
                    break; // Only record the first micro-failure to avoid spam
                }
            }
        }
    }

}
