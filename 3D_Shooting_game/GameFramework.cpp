//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "GraphicsPipeline.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));

	m_KeyMgr.Init();
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
    ::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer(); 

	m_pSceneMgr = new CSceneMgr();
	m_pSceneMgr->ChangeScene(new CTitleScene());

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);

	// 💡 BITMAPINFO 설정 제대로 복구
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = CLIENT_WIDTH;
	bmi.bmiHeader.biHeight = -CLIENT_HEIGHT;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);

	m_hBitmapFrameBuffer = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&g_pColorBuffer, NULL, 0);

	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);

	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);

	// 💡 전역 깊이 버퍼 g_pDepthBuffer 할당 (중복 할당 제거)
	if (g_pDepthBuffer != nullptr) delete[] g_pDepthBuffer;
	g_pDepthBuffer = new float[CLIENT_WIDTH * CLIENT_HEIGHT];
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, m_hDCFrameBuffer, 0, 0, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				::PostQuitMessage(0);
				return;
			}
			break;
	}

	if (m_pSceneMgr) m_pSceneMgr->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE) m_GameTimer.Stop();
		else m_GameTimer.Start();
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ReleaseObjects()
{
	if (m_pSceneMgr)
	{
		delete m_pSceneMgr;
		m_pSceneMgr = nullptr;
	}
}

void CGameFramework::OnDestroy()
{
	if (g_pDepthBuffer) delete[] g_pDepthBuffer;

	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

    if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput()
{
	m_KeyMgr.Update();

	float fMoveSpeed = 15.0f * m_GameTimer.GetTimeElapsed();
	float fRotationSpeed = 90.0f * m_GameTimer.GetTimeElapsed();

	if (m_pSceneMgr && m_pSceneMgr->m_pCurrentScene)
	{
		m_pSceneMgr->m_pCurrentScene->ProcessInput(&m_KeyMgr, m_GameTimer.GetTimeElapsed());
	}
}

void CGameFramework::FrameAdvance()
{    
	if (!m_bActive) return;

	m_GameTimer.Tick(0.0f);

	ProcessInput();

	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	if (m_pSceneMgr)
	{
		m_pSceneMgr->Animate(fTimeElapsed);
	}

	GraphicsPipeline::ClearBuffers(RGB(255, 255, 255));

	// 화면을 그릴 때 현재 씬에게서 얻어온 카메라를 넘겨줍니다.
	if (m_pSceneMgr && m_pSceneMgr->m_pCurrentScene)
	{
		CCamera* pMainCamera = m_pSceneMgr->m_pCurrentScene->GetCamera();
		m_pSceneMgr->Render(pMainCamera);
	}

	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


