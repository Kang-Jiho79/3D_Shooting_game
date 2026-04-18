#pragma once

#include "GameObject.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	CCamera						*m_pCamera = NULL;

	void SetPosition(float x, float y, float z);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	void Update(float fTimeElapsed=0.016f);

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(CCamera *pCamera);
};

#define BULLETS					30

class CTankPlayer : public CPlayer
{
public:
	CTankPlayer();
	virtual ~CTankPlayer();

	// 💡 파츠를 모두 독립된 3개의 객체로 관리합니다.
	CGameObject* m_pBody = NULL;   // 하체 (차체)
	CGameObject* m_pBumper = NULL;
	CGameObject* m_pTurret = NULL; // 상체 (포탑)
	CGameObject* m_pGun = NULL;    // 포신

	CBulletObject* m_ppBullets[BULLETS];

	float m_fBulletEffectiveRange = 500.0f;

	float m_fBodyYaw = 0.0f;  // 하체의 독립적인 Y축 회전 각도
	float m_fGunPitch = 0.0f; // 포신의 독립적인 X축 회전 (상하) 각도

public:
	virtual void Animate(float fElapsedTime);
	virtual void OnUpdateTransform();
	virtual void Render(CCamera* pCamera);

	void FireBullet(CGameObject* pSelectedObject);

	void RotateBody(float fAngle);   // A/D 키용: 하체 좌우 회전
	void RotateGun(float fAngle);    // 상하 방향키용: 포신 상하 각도
	void MoveBody(float fDistance);  // W/S 키용: 하체가 바라보는 방향으로 전진/후진
};

