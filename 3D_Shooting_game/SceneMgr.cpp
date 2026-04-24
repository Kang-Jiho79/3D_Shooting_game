#include "stdafx.h"
#include "SceneMgr.h"

CSceneMgr::CSceneMgr() {}

CSceneMgr::~CSceneMgr()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->ReleaseObjects();
        delete m_pCurrentScene;
    }
}

void CSceneMgr::ChangeScene(CScene* pNewScene)
{
    // 1. 기존 씬 정리
    if (m_pCurrentScene)
    {
        m_pCurrentScene->ReleaseObjects();
        delete m_pCurrentScene;
    }

    // 2. 새 씬 변환 및 초기화
    m_pCurrentScene = pNewScene;
    if (m_pCurrentScene)
    {
        m_pCurrentScene->m_pSceneMgr = this;
        m_pCurrentScene->BuildObjects();
    }
}

void CSceneMgr::Animate(float fElapsedTime)
{
    if (m_pCurrentScene) m_pCurrentScene->Animate(fElapsedTime);
}

void CSceneMgr::Render(CCamera* pCamera)
{
    if (m_pCurrentScene) m_pCurrentScene->Render(pCamera);
}

void CSceneMgr::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    if (m_pCurrentScene) m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

void CSceneMgr::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    if (m_pCurrentScene) m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}