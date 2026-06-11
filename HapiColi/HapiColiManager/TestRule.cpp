#include "TestRule.h"

namespace HapiColi
{
    bool ExpectedHitRule::Evaluate(const FrameData& frame, TestResult& outResult)
    {
        const ObjectData* subject = frame.GetObjectById(m_subjectId);
        if (!subject) return false;

        outResult.frame = frame.frame;
        outResult.targetId = m_subjectId;
        outResult.expected = "Hit " + m_targetId;

        if (subject->collision.isColliding && subject->collision.collidedWithId == m_targetId)
        {
            outResult.happy = true;
            outResult.actual = "Hit " + m_targetId;
            outResult.reason = "Success";
        }
        else
        {
            outResult.happy = false;
            outResult.actual = subject->collision.isColliding ? "Hit " + subject->collision.collidedWithId : "No Hit";
            outResult.reason = "Failed to hit target";
        }

        return true;
    }

    bool ExpectedNoHitRule::Evaluate(const FrameData& frame, TestResult& outResult)
    {
        const ObjectData* subject = frame.GetObjectById(m_subjectId);
        if (!subject) return false;

        outResult.frame = frame.frame;
        outResult.targetId = m_subjectId;
        outResult.expected = "No Hit " + m_targetId;

        if (subject->collision.isColliding && subject->collision.collidedWithId == m_targetId)
        {
            outResult.happy = false;
            outResult.actual = "Hit " + m_targetId;
            outResult.reason = "Unintended collision";
        }
        else
        {
            outResult.happy = true;
            outResult.actual = "Did not hit " + m_targetId;
            outResult.reason = "Success";
        }

        return true;
    }
}
