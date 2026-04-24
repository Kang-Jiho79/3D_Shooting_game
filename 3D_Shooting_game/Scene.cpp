#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "SceneMgr.h"

// =========================================================================
// 타이틀 씬 구현부
// =========================================================================
void CTitleScene::BuildObjects()
{
	// 1. 타이틀 전용 카메라 세팅 (플레이어 탱크 카메라와는 다른 고정된 위치 등)
	m_pTitleCamera = new CCamera();
	m_pTitleCamera->GenerateProjectionMatrix(1.01f, 5000.0f, 60.0f);
	m_pTitleCamera->SetViewport(0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, 0.0f, 1.0f);

	XMFLOAT3 xmf3Pos(0.0f, 10.0f, -20.0f);   // 약간 떨어진 위치
	XMFLOAT3 xmf3LookAt(0.0f, 0.0f, 0.0f);   // 0,0,0을 바라봄
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	m_pTitleCamera->Move(xmf3Pos);
	m_pTitleCamera->SetLookAt(xmf3LookAt, xmf3Up);
	m_pTitleCamera->GenerateViewMatrix();

	// 여기에 로고나 텍스트, 타이틀 메쉬 등을 추가로 세팅할 수 있습니다.
}

void CTitleScene::ReleaseObjects()
{
	if (m_pTitleCamera) delete m_pTitleCamera;
}

void CTitleScene::Animate(float fElapsedTime)
{
	// 타이틀 화면에서 카메라가 뱅글뱅글 도는 애니메이션 등을 추가 가능!
}

void CTitleScene::Render(CCamera* pCamera)
{
	// 글씨나 간단한 로고 오브젝트 렌더링
}

void CTitleScene::ProcessInput(KeyMgr* pKeyMgr, float fTimeElapsed)
{
	if (!pKeyMgr) return;

	// 💡 스페이스바를 누른(TAP) 순간
	if (pKeyMgr->GetKeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		// CSceneMgr의 포인터를 통해 PlayScene으로 교체 요청
		if (m_pSceneMgr)
		{
			m_pSceneMgr->ChangeScene(new CPlayScene());
		}
	}
}


CPlayScene::CPlayScene()
{
}

CPlayScene::~CPlayScene()
{
}

void CPlayScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void CPlayScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	m_pPlayer = new CTankPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 15.0f, -30.0f));

	float fHalfWidth = 45.0f, fHalfHeight = 10.0f, fHalfDepth = 110.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, fHalfHeight - 1.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	
	// 💡 벽면 색상 지정 (예: 약간 회색빛 도시 벽 느낌)
	// 바닥은 Mesh 내부에서 처리되므로, 이 설정값은 벽(좌/우/앞/뒤)에만 적용됩니다.
	m_pWallsObject->SetColor(RGB(120, 120, 120)); 

	/* ... 벽 세팅 m_pxmf4WallPlanes[0~5] 부분 그대로 둠 ... */
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	// 💡 기존 CExplosiveObject(cube) 10마리 생성/할당하던 코드는 모두 삭제!
	m_vObjects.clear();

	// 💡 AI가 달린 적 탱크 5마리 생성 및 랜덤 위치 배치
	for (int i = 0; i < 5; i++)
	{
		CTankEnemy* pEnemy = new CTankEnemy();
		// 좌우(-30~30), 앞뒤(10~40) 사이에 무작위로 위치시킵니다.
		pEnemy->SetPosition(RandF(-30.0f, 30.0f), 0.0f, RandF(10.0f, 40.0f));

		// AI 두뇌에 타겟(플레이어) 세팅!
		pEnemy->InitializeAI(m_pPlayer);

		// vector 컨테이너에 밀어넣기
		m_vObjects.push_back(pEnemy);
	}

	for (int i = 0; i < 10; i++)
		m_ppParticles[i] = new CParticleSystem();
}

void CPlayScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	// 💡 컨테이너 순회하여 메모리 삭제
	for (auto pObj : m_vObjects)
		if (pObj) delete pObj;
	m_vObjects.clear();

	for (int i = 0; i < 10; i++) delete m_ppParticles[i];
	if (m_pWallsObject) delete m_pWallsObject;
}

CGameObject *CPlayScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / pCamera->m_d3dViewport.Width) - 1) / pCamera->m_xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / pCamera->m_d3dViewport.Height) - 1) / pCamera->m_xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject *pNearestObject = NULL;
	for (auto pObj : m_vObjects)
	{
		if (!pObj->m_bActive) continue;
		float fHitDistance = FLT_MAX;
		nIntersected = pObj->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pObj;
		}
	}
	return(pNearestObject);
}

void CPlayScene::CheckObjectByObjectCollisions()
{
	for (auto pObj : m_vObjects) pObj->m_pObjectCollided = NULL;

	for (size_t i = 0; i < m_vObjects.size(); i++)
	{
		if (!m_vObjects[i]->m_bActive) continue;

		for (size_t j = (i + 1); j < m_vObjects.size(); j++)
		{
			if (!m_vObjects[j]->m_bActive) continue;

			if (m_vObjects[i]->m_xmOOBB.Intersects(m_vObjects[j]->m_xmOOBB))
			{
				m_vObjects[i]->m_pObjectCollided = m_vObjects[j];
				m_vObjects[j]->m_pObjectCollided = m_vObjects[i];
			}
		}
	}

	for (auto pObj : m_vObjects)
	{
		if (pObj->m_bActive && pObj->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = pObj->m_xmf3MovingDirection;
			float fMovingSpeed = pObj->m_fMovingSpeed;
			pObj->m_xmf3MovingDirection = pObj->m_pObjectCollided->m_xmf3MovingDirection;
			pObj->m_fMovingSpeed = pObj->m_pObjectCollided->m_fMovingSpeed;

			pObj->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			pObj->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;

			pObj->m_pObjectCollided->m_pObjectCollided = NULL;
			pObj->m_pObjectCollided = NULL;
		}
	}
}

void CPlayScene::CheckObjectByWallCollisions()
{
	// 💡 AI 역시크기를 고려해 넉넉하게 제한
	float fMarginX = 8.0f;
	float fMarginZ = 8.0f;
	
	float fLimitX = 45.0f - fMarginX;  
	float fLimitZ = 110.0f - fMarginZ; 

	for (auto pObj : m_vObjects)
	{
		if (!pObj->m_bActive) continue;

		XMFLOAT3 pos = pObj->GetPosition();
		bool bOutOfBounds = false;

		// 💡 X축 이탈 방지
		if (pos.x < -fLimitX) { pos.x = -fLimitX; bOutOfBounds = true; }
		else if (pos.x > fLimitX) { pos.x = fLimitX; bOutOfBounds = true; }

		// 💡 Z축 이탈 방지
		if (pos.z < -fLimitZ) { pos.z = -fLimitZ; bOutOfBounds = true; }
		else if (pos.z > fLimitZ) { pos.z = fLimitZ; bOutOfBounds = true; }

		if (bOutOfBounds)
		{
			pObj->SetPosition(pos.x, pos.y, pos.z);

			// 벽에 부딪히면 튕겨져 나오도록 방향을 반대로 역전
			pObj->m_xmf3MovingDirection.x *= -1.0f;
			pObj->m_xmf3MovingDirection.z *= -1.0f;
		}
	}
}

void CPlayScene::CheckPlayerByWallCollision()
{
	// 💡 탱크의 크기를 고려해 여유 폭(Margin)을 넓게 잡음 (기존 2.0 -> 8.0 등 탱크 크기에 맞게 조절)
	float fMarginX = 8.0f; // 탱크의 가로 절반 크기 정도
	float fMarginZ = 8.0f; // 탱크의 세로 절반 크기 정도
	
	float fLimitX = 45.0f - fMarginX;  // 맵의 절반 너비 45.0 - 탱크 여유분
	float fLimitZ = 110.0f - fMarginZ; // 맵의 절반 깊이 110.0 - 탱크 여유분

	XMFLOAT3 pos = m_pPlayer->GetPosition();

	if (pos.x < -fLimitX) pos.x = -fLimitX;
	if (pos.x > fLimitX) pos.x = fLimitX;
	if (pos.z < -fLimitZ) pos.z = -fLimitZ;
	if (pos.z > fLimitZ) pos.z = fLimitZ;

	// 바깥으로 나갔다면 제한된 좌표로 강제 복귀
	m_pPlayer->SetPosition(pos.x, pos.y, pos.z);
}

void CPlayScene::CheckObjectByBulletCollisions()
{
	CBulletObject** ppBullets = ((CTankPlayer*)m_pPlayer)->m_ppBullets;

	for (int j = 0; j < BULLETS; j++)
	{
		// 💡 1️⃣ 바닥 폭발 검사 (Bullet이 플래그를 켰으면 폭발)
		if (ppBullets[j]->m_bHitSurface)
		{
			SpawnParticle(ppBullets[j]->GetPosition());
			ppBullets[j]->m_bHitSurface = false; // 소모 처리
		}

		if (!ppBullets[j]->m_bActive) continue;

		// 💡 2️⃣ 몹(다른 오브젝트) 충돌 폭발 검사
		for (auto pObj : m_vObjects)
		{
			// 적탱크가 살아있고 총알과 OOBB 충돌 박스가 겹친다면
			if (pObj->m_bActive && pObj->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
			{
				pObj->m_bActive = false; // 맞은 적 탱크 사망 처리
				SpawnParticle(pObj->GetPosition()); // 화려한 폭발 파티클

				ppBullets[j]->m_bActive = false; // 총알 소멸
				break;
			}
		}
	}

	// 💡 3️⃣ 적군의 총알 충돌 및 바닥 폭발 파티클 처리
	for (auto pObj : m_vObjects)
	{
		CTankEnemy* pEnemy = dynamic_cast<CTankEnemy*>(pObj);
		if (!pEnemy) continue;

		for (int j = 0; j < ENEMY_BULLETS; j++)
		{
			// 적 총알이 바닥에 맞은 경우
			if (pEnemy->m_ppBullets[j]->m_bHitSurface)
			{
				SpawnParticle(pEnemy->m_ppBullets[j]->GetPosition());
				pEnemy->m_ppBullets[j]->m_bHitSurface = false; // 소모 처리
			}

			if (!pEnemy->m_ppBullets[j]->m_bActive) continue;

			// 적 총알이 플레이어 명중 시 OOBB 충돌 검사
			if (m_pPlayer && pEnemy->m_ppBullets[j]->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB))
			{
				// 피격 시 파티클 이펙트 생성
				SpawnParticle(pEnemy->m_ppBullets[j]->GetPosition());
				
				pEnemy->m_ppBullets[j]->m_bActive = false; // 총알 소멸
				
				
				// 필요하다면 이곳에 플레이어 데미지 처리 코드를 추가할 수 있습니다.
			}
		}
	}
}

void CPlayScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);
	for (auto pObj : m_vObjects)
		if (pObj->m_bActive) pObj->Animate(fElapsedTime);

	if (m_pPlayer)
	{
		m_pPlayer->Animate(fElapsedTime);
	}

	CheckPlayerByWallCollision();

	CheckObjectByWallCollisions();

	CheckObjectByObjectCollisions();

	CheckObjectByBulletCollisions();

	for (int i = 0; i < 10; i++)
		if (m_ppParticles[i]->m_bActive) m_ppParticles[i]->Animate(fElapsedTime);
}

void CPlayScene::Render(CCamera* pCamera)
{
	if (m_pWallsObject)
		m_pWallsObject->Render(pCamera);

	for (auto pObj : m_vObjects)
		if (pObj->m_bActive) pObj->Render(pCamera);

	if (m_pPlayer)
		m_pPlayer->Render(pCamera);

	for (int i = 0; i < 10; i++)
		m_ppParticles[i]->Render(pCamera);
}	

void CPlayScene::SpawnParticle(XMFLOAT3 pos)
{
	// 놀고 있는(꺼져있는) 파티클 찾아서 그 자리에서 빵!
	for (int i = 0; i < 10; i++) {
		if (!m_ppParticles[i]->m_bActive) {
			m_ppParticles[i]->Spawn(pos);
			break;
		}
	}
}

void CPlayScene::ProcessInput(KeyMgr* pKeyMgr, float fTimeElapsed)
{
	if (!m_pPlayer || !pKeyMgr) return;

	float fMoveSpeed = 15.0f * fTimeElapsed;
	float fRotationSpeed = 90.0f * fTimeElapsed;

	if (pKeyMgr->GetKeyState(KEY::W) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->MoveBody(fMoveSpeed);
	if (pKeyMgr->GetKeyState(KEY::S) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->MoveBody(-fMoveSpeed);

	if (pKeyMgr->GetKeyState(KEY::A) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateBody(-fRotationSpeed);
	if (pKeyMgr->GetKeyState(KEY::D) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateBody(fRotationSpeed);

	if (pKeyMgr->GetKeyState(KEY::LEFT) == KEY_STATE::HOLD) m_pPlayer->Rotate(0.0f, -fRotationSpeed, 0.0f);
	if (pKeyMgr->GetKeyState(KEY::RIGHT) == KEY_STATE::HOLD) m_pPlayer->Rotate(0.0f, fRotationSpeed, 0.0f);

	if (pKeyMgr->GetKeyState(KEY::UP) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateGun(-fRotationSpeed * 0.5f);
	if (pKeyMgr->GetKeyState(KEY::DOWN) == KEY_STATE::HOLD) ((CTankPlayer*)m_pPlayer)->RotateGun(fRotationSpeed * 0.5f);

	if (pKeyMgr->GetKeyState(KEY::CTRL) == KEY_STATE::TAP)
	{
		((CTankPlayer*)m_pPlayer)->FireBullet(nullptr);
	}

	m_pPlayer->Update(fTimeElapsed);
}

CCamera* CPlayScene::GetCamera()
{
	// 플레이어가 가진 카메라 객체를 리턴합니다.
	if (m_pPlayer) return m_pPlayer->m_pCamera;
	return nullptr;
}