#pragma once

#include "FrameData.h"
#include "TestResult.h"
#include <string>

namespace HapiColi
{
    class TestRule
    {
    public:
        virtual ~TestRule() = default;
        
        // Return true if rule is evaluated (either happy or unhappy).
        // Return false if the rule doesn't apply to this frame.
        virtual bool Evaluate(const FrameData& frame, TestResult& outResult) = 0;
        
        virtual std::string GetName() const = 0;
        virtual std::string GetSubjectId() const = 0;
        virtual std::string GetTargetId() const = 0;
    };

    // Example Rule: Expected an object to hit another object
    class ExpectedHitRule : public TestRule
    {
    public:
        ExpectedHitRule(const std::string& subjectId, const std::string& targetId)
            : m_subjectId(subjectId), m_targetId(targetId) {}

        bool Evaluate(const FrameData& frame, TestResult& outResult) override;
        std::string GetName() const override { return "ExpectedHitRule"; }
        std::string GetSubjectId() const override { return m_subjectId; }
        std::string GetTargetId() const override { return m_targetId; }

    private:
        std::string m_subjectId;
        std::string m_targetId;
    };

    // Example Rule: Expected an object to NOT hit another object
    class ExpectedNoHitRule : public TestRule
    {
    public:
        ExpectedNoHitRule(const std::string& subjectId, const std::string& targetId)
            : m_subjectId(subjectId), m_targetId(targetId) {}

        bool Evaluate(const FrameData& frame, TestResult& outResult) override;
        std::string GetName() const override { return "ExpectedNoHitRule"; }
        std::string GetSubjectId() const override { return m_subjectId; }
        std::string GetTargetId() const override { return m_targetId; }

    private:
        std::string m_subjectId;
        std::string m_targetId;
    };
}
