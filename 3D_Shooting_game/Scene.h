#pragma once

#include "GameObject.h"
#include "Player.h"
#include "KeyMgr.h"
#include <vector>

// -----------------------------------------------------------------------------
// 모든 씬의 공통 부모 클래스 (인터페이스 역할)
// -----------------------------------------------------------------------------

class CSceneMgr;

class CScene
{
public:
	CScene() {}
	virtual ~CScene() {}

	CSceneMgr* m_pSceneMgr = nullptr;

	// 모든 씬이 필수로 구현해야 하는 가상 함수들
	virtual void BuildObjects() = 0;
	virtual void ReleaseObjects() = 0;
	virtual void Animate(float fElapsedTime) = 0;
	virtual void Render(CCamera* pCamera) = 0;
	virtual void ProcessInput(KeyMgr* pKeyMgr, float fTimeElapsed) {}
	virtual CCamera* GetCamera() { return nullptr; }

	// 입력 처리는 필요한 씬에서만 선택적으로 재정의할 수 있도록 빈 함수로 둡니다.
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}
};

// -----------------------------------------------------------------------------
// [추가] 타이틀 씬 클래스
// -----------------------------------------------------------------------------
class CTitleScene : public CScene
{
public:
	CTitleScene() {}
	virtual ~CTitleScene() {}

	CCamera* m_pTitleCamera = nullptr; // 타이틀 화면을 비출 단독 카메라

	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;
	virtual void Animate(float fElapsedTime) override;
	virtual void Render(CCamera* pCamera) override;
	virtual void ProcessInput(KeyMgr* pKeyMgr, float fTimeElapsed) override;
	virtual CCamera* GetCamera() override { return m_pTitleCamera; }
};


// -----------------------------------------------------------------------------
// 실제 본 게임 로직을 담당하는 플레이 씬
// -----------------------------------------------------------------------------
class CPlayScene : public CScene
{
public:
	CPlayScene();
	virtual ~CPlayScene();

	CPlayer						*m_pPlayer = NULL;

	std::vector<CGameObject*>		m_vObjects;

	CWallsObject					*m_pWallsObject = NULL;
	CParticleSystem				*m_ppParticles[10];
	
	void SpawnParticle(XMFLOAT3 pos);
	
	// 부모의 순수 가상 함수 구현
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;
	virtual void Animate(float fElapsedTime) override;
	virtual void Render(CCamera* pCamera) override;
	virtual void ProcessInput(class KeyMgr* pKeyMgr, float fTimeElapsed) override;
	virtual CCamera* GetCamera() override;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;


	// 게임 전용 충돌 및 유틸 함수들
	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();

	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);
};

