#pragma once
#include "stdafx.h"

extern DWORD* g_pColorBuffer;
extern float* g_pDepthBuffer;

namespace GraphicsPipeline
{
	// 버퍼 초기화 및 화면 출력
	void ClearBuffers(DWORD dwColor);

	// 삼각형 래스터라이즈 및 Z버퍼 처리
	void DrawTriangle(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, DWORD color);
}