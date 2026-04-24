#include "stdafx.h"
#include "BehaviorTree.h"

// 💡 1. 시퀀스(AND): 끝까지 돌면서 하나라도 실패하면 FAILURE 리턴
BTState BTSequence::Evaluate()
{
    for (auto& child : m_Children)
    {
        BTState state = child->Evaluate();
        if (state == BTState::FAILURE) return BTState::FAILURE;
        if (state == BTState::RUNNING) return BTState::RUNNING;
    }
    return BTState::SUCCESS; // 전부 통과
}

// 💡 2. 셀렉터(OR): 끝까지 돌면서 하나라도 성공하면 SUCCESS 리턴
BTState BTSelector::Evaluate()
{
    for (auto& child : m_Children)
    {
        BTState state = child->Evaluate();
        if (state == BTState::SUCCESS) return BTState::SUCCESS;
        if (state == BTState::RUNNING) return BTState::RUNNING;
    }
    return BTState::FAILURE; // 전부 실패
}