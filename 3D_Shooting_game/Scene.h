#pragma once

#include "GameObject.h"
#include "Player.h"

class CScene
{
public:
	CScene();
	virtual ~CScene();

	CPlayer						*m_pPlayer = NULL;

	int							m_nObjects = 0;
	CGameObject					**m_ppObjects = NULL;

	CWallsObject				*m_pWallsObject = NULL;
	CParticleSystem			* m_ppParticles[10];
	
	void SpawnParticle(XMFLOAT3 pos);
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(CCamera* pCamera);

	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);
};

