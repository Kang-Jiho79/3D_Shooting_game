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

	BuildObjects(); 

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
    ::BitBlt(hDC, int(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX), int(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY), int(m_pPlayer->m_pCamera->m_d3dViewport.Width), int(m_pPlayer->m_pCamera->m_d3dViewport.Height), m_hDCFrameBuffer, int(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftX), int(m_pPlayer->m_pCamera->m_d3dViewport.TopLeftY), SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_CONTROL:
					((CTankPlayer *)m_pPlayer)->FireBullet(m_pSelectedObject);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
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

void CGameFramework::BuildObjects()
{
	m_pPlayer = new CTankPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetColor(RGB(0, 255, 0)); // 탱크는 보통 녹색 계열

	// 카메라 오프셋: 탱크를 좀 더 위쪽/뒤쪽에서 내려다보는 3인칭 뷰 (Back View)
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 15.0f, -30.0f));

	m_pScene = new CScene();
	m_pScene->BuildObjects();

	m_pScene->m_pPlayer = m_pPlayer;
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pPlayer) delete m_pPlayer;
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

	// 1. W/S (하체 기준으로 앞/뒤 이동)
	if (m_KeyMgr.GetKeyState(KEY::W) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->MoveBody(fMoveSpeed);
	if (m_KeyMgr.GetKeyState(KEY::S) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->MoveBody(-fMoveSpeed);

	// 2. A/D (하체만 좌우로 회전)
	if (m_KeyMgr.GetKeyState(KEY::A) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateBody(-fRotationSpeed);
	if (m_KeyMgr.GetKeyState(KEY::D) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateBody(fRotationSpeed);

	// 3. 방향키 좌/우 (상체와 카메라가 같이 회전)
	// CPlayer의 Rotate를 호출하면 카메라가 자동으로 따라옵니다.
	if (m_KeyMgr.GetKeyState(KEY::LEFT) == KEY_STATE::HOLD) m_pPlayer->Rotate(0.0f, -fRotationSpeed, 0.0f);
	if (m_KeyMgr.GetKeyState(KEY::RIGHT) == KEY_STATE::HOLD) m_pPlayer->Rotate(0.0f, fRotationSpeed, 0.0f);

	// 4. 방향키 상/하 (포신 상하 각도 조절)
	if (m_KeyMgr.GetKeyState(KEY::UP) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateGun(-fRotationSpeed * 0.5f);
	if (m_KeyMgr.GetKeyState(KEY::DOWN) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateGun(fRotationSpeed * 0.5f);

	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{    
    if (!m_bActive) return;

	m_GameTimer.Tick(0.0f);

	ProcessInput();

	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	m_pPlayer->Animate(fTimeElapsed);
	m_pScene->Animate(fTimeElapsed);

	GraphicsPipeline::ClearBuffers(RGB(255, 255, 255));

	m_pScene->Render(m_pPlayer->m_pCamera);

	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


