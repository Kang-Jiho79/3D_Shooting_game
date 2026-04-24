#pragma once
#include <vector>
#include <memory>

// 노드의 실행 결과 상태
enum class BTState
{
    SUCCESS,
    FAILURE,
    RUNNING
};

// ==========================================
// 1. 모든 BT 노드의 기본 인터페이스
// ==========================================
class BTNode
{
public:
    virtual ~BTNode() {}
    virtual BTState Evaluate() = 0;
};

// ==========================================
// 2. 자식 노드들을 가질 수 있는 컴포짓(Composite) 베이스
// ==========================================
class BTComposite : public BTNode
{
protected:
    std::vector<std::shared_ptr<BTNode>> m_Children;
public:
    void AddChild(std::shared_ptr<BTNode> child)
    {
        m_Children.push_back(child);
    }
};

// ==========================================
// 3. 시퀀스 (Sequence, AND 조건)
// - 자식 노드들을 순서대로 실행
// - 하나라도 실패(FAILURE)하면 즉시 해당 프레임 중단 및 FAILURE 반환
// - 모두 완료하면 SUCCESS 반환
// ==========================================
class BTSequence : public BTComposite
{
public:
    virtual BTState Evaluate() override;
};

// ==========================================
// 4. 셀렉터 (Selector, OR 조건 / Fallback)
// - 자식 노드들을 순서대로 실행
// - 하나라도 성공(SUCCESS)하면 즉시 해당 프레임 중단 및 SUCCESS 반환
// - 모두 실패하면 FAILURE 반환
// ==========================================
class BTSelector : public BTComposite
{
public:
    virtual BTState Evaluate() override;
};