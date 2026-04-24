#include "StdAfx.h"
#include "GameObject.h"
#include "BehaviorTree.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject() 
{ 
	m_pMesh = NULL; 
	m_xmf4x4World = Matrix4x4::Identity();
	
	m_dwColor = RGB(255, 255, 255);

	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fMovingSpeed = 0.0f;
	m_fMovingRange = 0.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 0.05f;
}

CGameObject::CGameObject(CMesh *pMesh) : CGameObject()
{
	m_pMesh = pMesh; 
}

CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z) 
{
	m_xmf4x4World._41 = x; 
	m_xmf4x4World._42 = y; 
	m_xmf4x4World._43 = z; 
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position) 
{ 
	m_xmf4x4World._41 = xmf3Position.x; 
	m_xmf4x4World._42 = xmf3Position.y; 
	m_xmf4x4World._43 = xmf3Position.z; 
}

XMFLOAT3 CGameObject::GetPosition() 
{ 
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); 
}

XMFLOAT3 CGameObject::GetLook() 
{ 	
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CGameObject::GetUp() 
{ 	
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{ 	
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookTo, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Animate(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);	

	UpdateBoundingBox();
}

void CGameObject::Render(CCamera *pCamera)
{
	if (m_pMesh)
	{
		if (pCamera->IsInFrustum(m_xmOOBB))
		{
			m_pMesh->Render(m_xmf4x4World, m_dwColor, pCamera);
		}
	}
}

void CGameObject::GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4World) * xmmtxView);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	xmvPickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	xmvPickRayDirection = XMVector3TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

int CGameObject::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float *pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMVECTOR xmvPickRayOrigin, xmvPickRayDirection;
		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);
		nIntersected = m_pMesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

void CWallsObject::Render(CCamera* pCamera)
{
	if (m_pMesh)
	{
		// IsInFrustum 검사를 생략하고 무조건 Render 호출
		m_pMesh->Render(m_xmf4x4World, m_dwColor, pCamera);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh *CExplosiveObject::m_pExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::PrepareExplosion()
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_pExplosionMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bBlowingUp)
	{
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	}
	else
	{
		CGameObject::Animate(fElapsedTime);
	}
}

void CExplosiveObject::Render(CCamera* pCamera)
{
	if (m_bBlowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (m_pExplosionMesh)
			{
				m_pExplosionMesh->Render(m_pxmf4x4Transforms[i], m_dwColor, pCamera);
			}
		}
	}
	else
	{
		CGameObject::Render(pCamera);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBulletObject::CBulletObject(float fEffectiveRange)
{
	m_fBulletEffectiveRange = fEffectiveRange;
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{ 
	m_xmf3FirePosition = xmf3FirePosition; 
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Animate(float fElapsedTime)
{
	// 💡 1. 현재 총알의 실제 3D 속도 벡터 계산 (방향 * 속도)
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3MovingDirection, m_fMovingSpeed, false);

	// 💡 2. 중력 가속도 적용 (매 프레임 아래로 떨어지는 힘 누적)
	xmf3Velocity.y -= GRAVITY * fElapsedTime;

	// 💡 3. 변경된 속도 벡터에서 새로운 방향과 속도 추출
	m_fMovingSpeed = Vector3::Length(xmf3Velocity);
	m_xmf3MovingDirection = Vector3::Normalize(xmf3Velocity);

	// 이동 거리 = 새로운 속도 * 시간
	float fDistance = m_fMovingSpeed * fElapsedTime;

	// 포탄 회전
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, m_fRotationSpeed * fElapsedTime, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

	// 이동 적용부
	XMFLOAT3 xmf3Movement = Vector3::ScalarProduct(m_xmf3MovingDirection, fDistance, false);
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Movement);
	SetPosition(xmf3Position);

	UpdateBoundingBox();

	// 땅에 떨어지면(y <= 0) 없애거나 거리를 초과하면 없앰 (탱크 바닥 조건)
	if (GetPosition().y <= 0.0f || Vector3::Distance(m_xmf3FirePosition, GetPosition()) > m_fBulletEffectiveRange)
	{
		if (GetPosition().y <= 0.0f) m_bHitSurface = true; // 💡 바닥에 맞으면 파티클 터지라고 신호 보냄
		SetActive(false);
	}
}

// -----------------------------------------------------
// 💡 아래에 CParticleSystem 전체 구현을 복사해 넣습니다!
CParticleSystem::CParticleSystem()
{
	SetMesh(new CCubeMesh(0.4f, 0.4f, 0.4f)); // 파편은 작은 큐브
	m_bActive = false;
}
CParticleSystem::~CParticleSystem() {}

void CParticleSystem::Spawn(XMFLOAT3 xmf3Position)
{
	m_bActive = true;
	m_fAge = 0.0f;

	// 폭발 색상 배열 (빨강, 주황, 노랑)
	DWORD colors[3] = { RGB(255, 0, 0), RGB(255, 128, 0), RGB(255, 255, 0) };

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		m_xmf3Positions[i] = xmf3Position;

		// 랜덤 방사형 속도
		float fx = ((rand() % 100) / 50.0f) - 1.0f;
		float fy = ((rand() % 100) / 50.0f) + 0.5f; // 위쪽으로 튀어오름
		float fz = ((rand() % 100) / 50.0f) - 1.0f;
		XMFLOAT3 dir = Vector3::Normalize(XMFLOAT3(fx, fy, fz));
		float speed = 10.0f + (rand() % 20); // 10~30 사이의 파편 속도

		m_xmf3Velocities[i] = Vector3::ScalarProduct(dir, speed, false);
		m_dwColors[i] = colors[rand() % 3];
	}
}

void CParticleSystem::Animate(float fElapsedTime)
{
	if (!m_bActive) return;

	m_fAge += fElapsedTime;
	if (m_fAge > m_fLifeTime) {
		m_bActive = false;
		return;
	}

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		// 중력 가속도
		m_xmf3Velocities[i].y -= 30.0f * fElapsedTime;

		// 이동
		XMFLOAT3 move = Vector3::ScalarProduct(m_xmf3Velocities[i], fElapsedTime, false);
		m_xmf3Positions[i] = Vector3::Add(m_xmf3Positions[i], move);

		// 바닥에 통통 튕기거나 멈추기
		if (m_xmf3Positions[i].y < 0.0f) {
			m_xmf3Positions[i].y = 0.0f;
			m_xmf3Velocities[i].y *= -0.5f; // 바닥에 닿으면 반발로 살짝 튐
		}
	}
}

void CParticleSystem::Render(CCamera* pCamera)
{
	if (!m_bActive || !m_pMesh) return;

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		// 각 파편의 위치만 적용하여 다중 렌더링
		XMFLOAT4X4 matWorld = Matrix4x4::Translate(m_xmf3Positions[i].x, m_xmf3Positions[i].y, m_xmf3Positions[i].z);
		m_pMesh->Render(matWorld, m_dwColors[i], pCamera);
	}
}


// ==============================================================================
// CTankEnemy 구현
// ==============================================================================
CTankEnemy::CTankEnemy()
{
	SetMesh(NULL); // 본체는 투명한 기준축

	// 1. 하체(Body)
	m_pBody = new CGameObject();
	m_pBody->SetMesh(new CCubeMesh(6.0f, 2.0f, 10.0f));
	m_pBody->SetColor(RGB(150, 0, 0)); // 어두운 빨간색

	// 2. 상체(Turret)
	m_pTurret = new CGameObject();
	m_pTurret->SetMesh(new CCubeMesh(4.0f, 2.0f, 4.0f));
	m_pTurret->SetColor(RGB(255, 50, 50)); // 밝은 빨간색

	// 3. 포신(Gun)
	m_pGun = new CGameObject();
	m_pGun->SetMesh(new CCubeMesh(0.5f, 0.5f, 6.0f));
	m_pGun->SetColor(RGB(64, 64, 64)); // 회색

	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		m_ppBullets[i] = new CBulletObject(200.0f);
		m_ppBullets[i]->SetMesh(new CCubeMesh(0.5f, 0.5f, 1.0f));
		m_ppBullets[i]->SetColor(RGB(255, 0, 0)); // 빨간 총알
		m_ppBullets[i]->SetMovingSpeed(15.0f);
		m_ppBullets[i]->SetActive(false);
	}
}

CTankEnemy::~CTankEnemy()
{
	if (m_pBody) delete m_pBody;
	if (m_pTurret) delete m_pTurret;
	if (m_pGun) delete m_pGun;
	for (int i = 0; i < ENEMY_BULLETS; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CTankEnemy::OnUpdateTransform()
{
	// 1. 하체 행렬 (기준축 회전)
	XMFLOAT4X4 xmtxBodyRot = Matrix4x4::RotationYawPitchRoll(0.0f, m_fBodyYaw, 0.0f);
	XMFLOAT4X4 xmtxTrans = Matrix4x4::Translate(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	if (m_pBody) m_pBody->m_xmf4x4World = Matrix4x4::Multiply(xmtxBodyRot, xmtxTrans);

	// 2. 상체 행렬 (하체 위치에서 독립적인 포탑 회전 + Y축 오프셋)
	XMFLOAT4X4 xmtxTurretRot = Matrix4x4::RotationYawPitchRoll(0.0f, m_fTurretYaw, 0.0f);
	XMFLOAT4X4 xmtxTurretTrans = Matrix4x4::Translate(0.0f, 2.0f, 0.0f);
	XMFLOAT4X4 xmtxTurretLocal = Matrix4x4::Multiply(xmtxTurretRot, xmtxTurretTrans);
	if (m_pTurret && m_pBody) m_pTurret->m_xmf4x4World = Matrix4x4::Multiply(xmtxTurretLocal, m_pBody->m_xmf4x4World);

	// 3. 포신 행렬
	XMFLOAT4X4 xmtxGunRot = Matrix4x4::RotationYawPitchRoll(m_fGunPitch, 0.0f, 0.0f);
	XMFLOAT4X4 xmtxGunTrans = Matrix4x4::Translate(0.0f, 0.0f, 3.0f);
	XMFLOAT4X4 xmtxGunLocal = Matrix4x4::Multiply(xmtxGunRot, xmtxGunTrans);
	if (m_pGun && m_pTurret) m_pGun->m_xmf4x4World = Matrix4x4::Multiply(xmtxGunLocal, m_pTurret->m_xmf4x4World);

	// 바운딩 박스 갱신 (전체 충돌 영역)
	m_xmOOBB = m_pBody->m_xmOOBB;
}

void CTankEnemy::Animate(float fElapsedTime)
{
	// 💡 매 프레임마다 공격 쿨타임을 감소
	if (m_fFireCooldownTimer > 0.0f)
	{
		m_fFireCooldownTimer -= fElapsedTime;
	}

	if (m_bActive && m_BTRoot)
	{
		m_BTRoot->Evaluate(); // 💡 여기서 매 프레임 AI 판단이 일어납니다!
	}

	CGameObject::Animate(fElapsedTime);
	OnUpdateTransform(); // 계층 구조 행렬 업데이트

	if (m_pBody) m_pBody->Animate(fElapsedTime);
	if (m_pTurret) m_pTurret->Animate(fElapsedTime);
	if (m_pGun) m_pGun->Animate(fElapsedTime);

	for (int i = 0; i < ENEMY_BULLETS; i++)
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Animate(fElapsedTime);
}

void CTankEnemy::Render(CCamera* pCamera)
{
	if (m_pBody) m_pBody->Render(pCamera);
	if (m_pTurret) m_pTurret->Render(pCamera);
	if (m_pGun) m_pGun->Render(pCamera);

	for (int i = 0; i < ENEMY_BULLETS; i++)
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(pCamera);
}

void CTankEnemy::RotateBody(float fAngle) { m_fBodyYaw += fAngle; }
void CTankEnemy::RotateTurret(float fAngle) { m_fTurretYaw += fAngle; }
void CTankEnemy::MoveBody(float fDistance)
{
	XMFLOAT3 xmf3BodyLook = XMFLOAT3(sinf(XMConvertToRadians(m_fBodyYaw)), 0.0f, cosf(XMConvertToRadians(m_fBodyYaw)));
	XMFLOAT3 xmf3Shift = Vector3::ScalarProduct(Vector3::Normalize(xmf3BodyLook), fDistance);
	Move(xmf3Shift, 1.0f);
}

void CTankEnemy::FireBullet()
{
	// 💡 쿨타임이 0보다 크면(아직 돌고 있으면) 총알을 발사하지 않고 함수 종료
	if (m_fFireCooldownTimer > 0.0f) return;

	OnUpdateTransform();
	CBulletObject* pBullet = nullptr;
	for (int i = 0; i < ENEMY_BULLETS; i++)
	{
		if (!m_ppBullets[i]->m_bActive) { pBullet = m_ppBullets[i]; break; }
	}
	
	if (pBullet && m_pGun)
	{
		XMFLOAT3 xmf3GunPos = m_pGun->GetPosition();
		XMFLOAT3 xmf3GunDir = m_pGun->GetLook();
		XMFLOAT3 xmf3FirePos = Vector3::Add(xmf3GunPos, Vector3::ScalarProduct(xmf3GunDir, 4.0f));

		pBullet->m_xmf4x4World = m_pGun->m_xmf4x4World; // 회전값 복사
		pBullet->SetFirePosition(xmf3FirePos);
		pBullet->SetMovingDirection(xmf3GunDir);
		pBullet->SetActive(true);

		// 💡 한 발 발사 후 1.5초(원하는 초만큼 설정) 동안 쿨타임 세팅
		m_fFireCooldownTimer = 1.5f; 
	}
}

// ==========================================
// AI 탱크용 BT 커스텀 노드들
// ==========================================

// 1. 조건: 거리가 X 이내인가?
class CheckDistanceNode : public BTNode {
	CTankEnemy* tank; float range;
public:
	CheckDistanceNode(CTankEnemy* t, float r) : tank(t), range(r) {}
	virtual BTState Evaluate() override {
		if (!tank->m_pTargetPlayer) return BTState::FAILURE;
		float dist = Vector3::Distance(tank->GetPosition(), tank->m_pTargetPlayer->GetPosition());
		return (dist <= range) ? BTState::SUCCESS : BTState::FAILURE;
	}
};

// 2. 조건: 포탑이 플레이어를 정조준(일치) 중인가?
class CheckAimNode : public BTNode {
	CTankEnemy* tank; float epsilon;
public:
	CheckAimNode(CTankEnemy* t, float e) : tank(t), epsilon(e) {}
	virtual BTState Evaluate() override {
		if (!tank->m_pTargetPlayer || !tank->m_pGun) return BTState::FAILURE;
		XMFLOAT3 dirToPlayer = Vector3::Normalize(Vector3::Subtract(tank->m_pTargetPlayer->GetPosition(), tank->GetPosition()));
		float dot = Vector3::DotProduct(tank->m_pGun->GetLook(), dirToPlayer);
		return (dot > epsilon) ? BTState::SUCCESS : BTState::FAILURE; // 허용 오차 내 정조준
	}
};

// 3. 행동: 공격 (Fire)
class ActionFireNode : public BTNode {
	CTankEnemy* tank;
public:
	ActionFireNode(CTankEnemy* t) : tank(t) {}
	virtual BTState Evaluate() override {
		tank->FireBullet();
		return BTState::SUCCESS;
	}
};

// 4. 행동: 포탑을 플레이어 방향으로 회전
class ActionAimNode : public BTNode {
	CTankEnemy* tank; float fTimeElapsed;
public:
	ActionAimNode(CTankEnemy* t, float dt) : tank(t), fTimeElapsed(dt) {}
	virtual BTState Evaluate() override {
		if (!tank->m_pTargetPlayer || !tank->m_pGun) return BTState::FAILURE;

		// 💡 단순 목표 각도 계산 후 천천히 돌림
		XMFLOAT3 dirToPlayer = Vector3::Normalize(Vector3::Subtract(tank->m_pTargetPlayer->GetPosition(), tank->GetPosition()));
		XMFLOAT3 curLook = tank->m_pGun->GetLook();

		// 내적/외적을 써서 좌/우 중 어느 방향으로 돌릴지 결정
		XMFLOAT3 cross = Vector3::CrossProduct(curLook, dirToPlayer);
		if (cross.y > 0) tank->RotateTurret(90.0f * 0.016f); // 좌회전
		else tank->RotateTurret(-90.0f * 0.016f); // 우회전

		return BTState::RUNNING; // 아직 도는 중
	}
};

// 5. 행동: 무작위 방향으로 방황 이동
class ActionWanderNode : public BTNode {
	CTankEnemy* tank; float fTimeElapsed;
public:
	ActionWanderNode(CTankEnemy* t, float dt) : tank(t), fTimeElapsed(dt) {}
	virtual BTState Evaluate() override {
		tank->m_fWanderTimer -= fTimeElapsed;
		if (tank->m_fWanderTimer <= 0.0f) {
			tank->m_fWanderYaw = RandF(0.0f, 360.0f); // 3초마다 랜덤 각도
			tank->m_fWanderTimer = 3.0f;
		}

		// 목표 각도로 서서히 몸체 돌리며 직진 (단순화: 일단 해당 각도로 즉시 설정)
		tank->m_fBodyYaw = tank->m_fWanderYaw;
		tank->MoveBody(5.0f * fTimeElapsed); // 천천히 이동

		return BTState::RUNNING;
	}
};


// ===============================================
// 두뇌 조립 함수 및 업데이트 호출부
// ===============================================
void CTankEnemy::InitializeAI(CPlayer* pPlayer)
{
	m_pTargetPlayer = pPlayer;
	m_fWanderTimer = 0.0f;

	// 트리 생성
	auto rootSelector = std::make_shared<BTSelector>();

	// (1) 조준 상태면 사격
	auto seqFire = std::make_shared<BTSequence>();
	seqFire->AddChild(std::make_shared<CheckDistanceNode>(this, 30.0f)); // 20 내부
	seqFire->AddChild(std::make_shared<CheckAimNode>(this, 0.98f)); // 거진 일치 (1.0 = 완벽)
	seqFire->AddChild(std::make_shared<ActionFireNode>(this));

	// (2) 사정거리 안으로 들어오면 조준 시도 (포탑 회전)
	auto seqAim = std::make_shared<BTSequence>();
	seqAim->AddChild(std::make_shared<CheckDistanceNode>(this, 30.0f));
	seqAim->AddChild(std::make_shared<ActionAimNode>(this, 0.016f));

	// (3) 그 외에는 방황
	auto actWander = std::make_shared<ActionWanderNode>(this, 0.016f);

	// 트리 연결 (우선순위 순서대로 추가)
	rootSelector->AddChild(seqFire);
	rootSelector->AddChild(seqAim);
	rootSelector->AddChild(actWander);

	m_BTRoot = rootSelector;
}

