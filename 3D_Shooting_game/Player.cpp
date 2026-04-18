#include "stdafx.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer::CPlayer()
{
	m_pCamera = new CCamera();
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, 60.0f);
	m_pCamera->SetViewport(0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, 0.0f, 1.0f);

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, true);
	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::Update(float fTimeElapsed)
{
	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);
}

void CPlayer::Animate(float fElapsedTime)
{
	OnUpdateTransform();

	CGameObject::Animate(fElapsedTime);
}

void CPlayer::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(CCamera *pCamera)
{
	CGameObject::Render(pCamera);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//

CTankPlayer::CTankPlayer()
{
	// CTankPlayer 본체는 껍데기(카메라 중심축)로만 씁니다.
	SetMesh(NULL);

	// 1. 하체(Body) 생성 (길이 10)
	m_pBody = new CGameObject();
	m_pBody->SetMesh(new CCubeMesh(6.0f, 2.0f, 10.0f));
	m_pBody->SetColor(RGB(0, 128, 0));

	// 💡 1-1. 전면 범퍼 생성 (노란색/밝은색으로 포인트를 주어 앞을 표시!)
	m_pBumper = new CGameObject();
	// 차체 폭보다 약간 작고 얇은 박스
	m_pBumper->SetMesh(new CCubeMesh(4.0f, 1.0f, 1.0f));
	m_pBumper->SetColor(RGB(255, 255, 0)); // 노란색

	// 2. 상체(Turret) 생성
	m_pTurret = new CGameObject();
	m_pTurret->SetMesh(new CCubeMesh(4.0f, 2.0f, 4.0f));
	m_pTurret->SetColor(RGB(0, 150, 0));

	// 3. 포신(Gun) 생성
	m_pGun = new CGameObject();
	m_pGun->SetMesh(new CCubeMesh(0.5f, 0.5f, 6.0f));
	m_pGun->SetColor(RGB(64, 64, 64));

	for (int i = 0; i < BULLETS; i++)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(new CCubeMesh(0.5f, 0.5f, 1.0f));
		m_ppBullets[i]->SetMovingSpeed(20.0f);
		m_ppBullets[i]->SetActive(false);
	}
}

CTankPlayer::~CTankPlayer()
{
	if (m_pBody) delete m_pBody;
	if (m_pBumper) delete m_pBumper;
	if (m_pTurret) delete m_pTurret;
	if (m_pGun) delete m_pGun;
	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CTankPlayer::Animate(float fElapsedTime)
{
	CPlayer::Animate(fElapsedTime);

	if (m_pBody) m_pBody->Animate(fElapsedTime);
	if (m_pBumper) m_pBumper->Animate(fElapsedTime);
	if (m_pTurret) m_pTurret->Animate(fElapsedTime);
	if (m_pGun) m_pGun->Animate(fElapsedTime);

	for (int i = 0; i < BULLETS; i++)
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Animate(fElapsedTime);
}

void CTankPlayer::OnUpdateTransform()
{
	// 💡 1. 본체(상체 + 카메라) 월드 행렬 업데이트
	CPlayer::OnUpdateTransform();

	// 2. 하체(Body) 행렬 연산
	XMFLOAT4X4 xmtxBodyRot = Matrix4x4::RotationYawPitchRoll(0.0f, m_fBodyYaw, 0.0f);
	XMFLOAT4X4 xmtxTrans = Matrix4x4::Translate(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	if (m_pBody) m_pBody->m_xmf4x4World = Matrix4x4::Multiply(xmtxBodyRot, xmtxTrans);

	// 💡 2-1. 범퍼 행렬 연산 (하체를 부모로 삼아 앞쪽(Z축 방향) 상단에 부착)
	XMFLOAT4X4 xmtxBumperLocal = Matrix4x4::Translate(0.0f, 0.5f, 5.5f); // Z축으로 5.0f 전진 (차체 길이가 10이므로 끝단에 붙음)
	if (m_pBumper && m_pBody)
		m_pBumper->m_xmf4x4World = Matrix4x4::Multiply(xmtxBumperLocal, m_pBody->m_xmf4x4World);



	// 💡 3. 상체(Turret) 행렬 연산: CPlayer(현재 클래스)의 회전과 위치를 그대로 쓰되 Y축으로 2.0f 오프셋
	XMFLOAT4X4 xmtxTurretTrans = Matrix4x4::Translate(0.0f, 2.0f, 0.0f);
	if (m_pTurret) m_pTurret->m_xmf4x4World = Matrix4x4::Multiply(xmtxTurretTrans, m_xmf4x4World);

	// 💡 4. 포신(Gun) 행렬 연산: 상체 행렬을 부모로 하여 상하 각도(GunPitch)와 Z축 오프셋 적용
	XMFLOAT4X4 xmtxGunRot = Matrix4x4::RotationYawPitchRoll(m_fGunPitch, 0.0f, 0.0f);
	XMFLOAT4X4 xmtxGunTrans = Matrix4x4::Translate(0.0f, 0.0f, 3.0f);
	XMFLOAT4X4 xmtxGunLocal = Matrix4x4::Multiply(xmtxGunRot, xmtxGunTrans);
	if (m_pGun && m_pTurret) m_pGun->m_xmf4x4World = Matrix4x4::Multiply(xmtxGunLocal, m_pTurret->m_xmf4x4World);
}

void CTankPlayer::Render(CCamera* pCamera)
{
	if (m_pBody) m_pBody->Render(pCamera);
	if (m_pBumper) m_pBumper->Render(pCamera);
	if (m_pTurret) m_pTurret->Render(pCamera);
	if (m_pGun) m_pGun->Render(pCamera);

	for (int i = 0; i < BULLETS; i++)
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(pCamera);
}

void CTankPlayer::RotateBody(float fAngle) { m_fBodyYaw += fAngle; }
void CTankPlayer::RotateGun(float fAngle)
{
	m_fGunPitch += fAngle;

	if (m_fGunPitch > 10.0f) m_fGunPitch = 10.0f;
	if (m_fGunPitch < -60.0f) m_fGunPitch = -60.0f;
}
void CTankPlayer::MoveBody(float fDistance)
{
	// 하체의 앞방향(Look 벡터) 계산
	XMFLOAT3 xmf3BodyLook = XMFLOAT3(sinf(XMConvertToRadians(m_fBodyYaw)), 0.0f, cosf(XMConvertToRadians(m_fBodyYaw)));
	XMFLOAT3 xmf3Shift = Vector3::ScalarProduct(Vector3::Normalize(xmf3BodyLook), fDistance);
	Move(xmf3Shift, false);
}

void CTankPlayer::FireBullet(CGameObject* pSelectedObject)
{
	OnUpdateTransform();
	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!m_ppBullets[i]->m_bActive)
		{
			pBulletObject = m_ppBullets[i];
			break;
		}
	}
	if (pBulletObject && m_pGun)
	{
		XMFLOAT3 xmf3GunPos = m_pGun->GetPosition();
		XMFLOAT3 xmf3GunDir = m_pGun->GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3GunPos, Vector3::ScalarProduct(xmf3GunDir, 4.0f));

		pBulletObject->m_xmf4x4World = m_pGun->m_xmf4x4World; // 상하 회전값 복사
		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3GunDir);
		pBulletObject->SetActive(true);
	}
}