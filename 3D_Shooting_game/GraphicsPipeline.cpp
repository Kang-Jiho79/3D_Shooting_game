#include "stdafx.h"
#include "GraphicsPipeline.h"

// 전역 변수 메모리 실체화
DWORD* g_pColorBuffer = nullptr;
float* g_pDepthBuffer = nullptr;

namespace GraphicsPipeline
{
	void ClearBuffers(DWORD dwColor)
	{
		if (!g_pColorBuffer || !g_pDepthBuffer) return;

		// Z버퍼 무한대 초기화
		std::fill_n(g_pDepthBuffer, CLIENT_WIDTH * CLIENT_HEIGHT, FLT_MAX);

		// Color 버퍼 배경색 초기화
		DWORD clearColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		std::fill_n(g_pColorBuffer, CLIENT_WIDTH * CLIENT_HEIGHT, clearColor);
	}

	void DrawTriangle(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, DWORD color)
	{
		if (!g_pColorBuffer || !g_pDepthBuffer) return;

		DWORD dibColor = RGB(GetBValue(color), GetGValue(color), GetRValue(color));

		int minX = max(0, min((int)v0.x, min((int)v1.x, (int)v2.x)));
		int minY = max(0, min((int)v0.y, min((int)v1.y, (int)v2.y)));
		int maxX = min(CLIENT_WIDTH - 1, max((int)v0.x, max((int)v1.x, (int)v2.x)));
		int maxY = min(CLIENT_HEIGHT - 1, max((int)v0.y, max((int)v1.y, (int)v2.y)));

		auto edgeFunc = [](const XMFLOAT3& a, const XMFLOAT3& b, float cx, float cy) {
			return (cx - a.x) * (b.y - a.y) - (cy - a.y) * (b.x - a.x);
			};

		float area = edgeFunc(v0, v1, v2.x, v2.y);
		if (area == 0.0f) return;

		for (int y = minY; y <= maxY; ++y)
		{
			for (int x = minX; x <= maxX; ++x)
			{
				float px = (float)x + 0.5f;
				float py = (float)y + 0.5f;

				float w0 = edgeFunc(v1, v2, px, py) / area;
				float w1 = edgeFunc(v2, v0, px, py) / area;
				float w2 = edgeFunc(v0, v1, px, py) / area;

				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
					int bufferIndex = y * CLIENT_WIDTH + x;

					if (z >= 0.0f && z <= 1.0f && z < g_pDepthBuffer[bufferIndex])
					{
						g_pDepthBuffer[bufferIndex] = z;
						g_pColorBuffer[bufferIndex] = dibColor;
					}
				}
			}
		}
	}
}