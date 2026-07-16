#include "TestRule.h"
#include "../HapiColi.h"
#include "HapiColiManager.h"
#include "Recorder.h"

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

    bool ExpectedPartialHitRule::Evaluate(const FrameData& frame, TestResult& outResult)
    {
        const ObjectData* subject = frame.GetObjectById(m_subjectId);
        if (!subject) return false;

        outResult.frame = frame.frame;
        outResult.targetId = m_subjectId;
        outResult.expected = "Partially Hit " + m_targetId;

        // Check if there is ANY frame in the entire recording where subject collided with targetId
        bool hasAnyHit = false;
        auto manager = HapiColi::GetInstance().GetManager();
        if (manager)
        {
            const auto& frames = manager->GetRecorder()->GetRecordedFrames();
            for (const auto& f : frames)
            {
                const ObjectData* s = f.GetObjectById(m_subjectId);
                if (s && s->collision.isColliding && s->collision.collidedWithId == m_targetId)
                {
                    hasAnyHit = true;
                    break;
                }
            }
        }

        if (hasAnyHit)
        {
            outResult.happy = true;
            outResult.actual = "Partially Hit " + m_targetId;
            outResult.reason = "Success";
        }
        else
        {
            outResult.happy = false;
            outResult.actual = "No Hit at all";
            outResult.reason = "Failed to hit target even partially";
        }

        return true;
    }
}
