#pragma once
#include "Scene.h"

class CSceneMgr
{
public:
    CSceneMgr();
    ~CSceneMgr();

    // 현재 활성화된 씬
    CScene* m_pCurrentScene = nullptr;

    // 씬 교체 함수
    void ChangeScene(CScene* pNewScene);

    // 프레임워크에서 호출해줄 함수들을 매니저가 받아서 현재 씬으로 토스
    void Animate(float fElapsedTime);
    void Render(CCamera* pCamera);
    void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

