#pragma once

#include "Camera.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CVertex
{
public:
	CVertex() { }
	CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }

	XMFLOAT3					m_xmf3Position;
};

struct CProjectedPolygon
{
	POINT	ptVertices[10];
	int		nVertices;
	float	fDepth;		// 렌더링 시점의 평균 Z(깊이)값
	COLORREF color;

	// Z값이 큰(멀리 있는) 폴리곤부터 먼저 그리도록 내림차순 정렬
	bool operator<(const CProjectedPolygon& other) const {
		return fDepth > other.fDepth;
	}
};

class CPolygon
{
public:
	CPolygon(int nVertices);
	virtual ~CPolygon();

	int							m_nVertices;
	CVertex						*m_pVertices;

	// 💡 추가: 폴리곤 고유 색상 속성
	bool						m_bHasColor; 
	DWORD						m_dwColor;   

	void SetVertex(int nIndex, CVertex& vertex);
};

class CMesh
{
public:
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int							m_nReferences;

	int							m_nPolygons;
	CPolygon					**m_ppPolygons;

public:
	BoundingOrientedBox			m_xmOOBB;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

public:
	void SetPolygon(int nIndex, CPolygon *pPolygon);
	virtual void Render(XMFLOAT4X4& xmf4x4World, DWORD dwColor, CCamera* pCamera);

	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float *pfNearHitDistance);
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float *pfNearHitDistance);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CCubeMesh();
};

class CWallMesh : public CMesh
{
public:
	CWallMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f, int nSubRects = 20);
	virtual ~CWallMesh();
};

class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f);
	virtual ~CAirplaneMesh();
};



