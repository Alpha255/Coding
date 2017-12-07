#include "Tessellator.h"

extern D3DGraphic* g_Renderer;

void Tessellator::CreateLookupTable()
{	
	uint32_t halfFactor = s_MaxFactor / 2U;
	
	int32_t finalPointPosTable[s_MaxFactor / 2 + 1] = { 0 };
	finalPointPosTable[1] = halfFactor;

	/// Init final_Point_Position_Table
	for (uint32_t i = 2U; i < halfFactor + 1; ++i)
	{
		int32_t level = 0;
		for (;;)
		{
			if (0 == (((i - 2) - ((1UL << level) - 1)) & ((1UL << (level + 1)) - 1)))
			{
				break;
			}
			++level;
		}

		finalPointPosTable[i] = ((halfFactor >> 1) + ((i - 2) - ((1UL << level) - 1))) >> (level + 1);
	}

	/// Init inside_Point_Index_Table / outside_Point_Index_Table
	for (uint32_t i = 0U; i <= halfFactor; ++i)
	{
		for (uint32_t j = 0U; j <= halfFactor; ++j)
		{
			m_InsidePointIndex[i][j][0] = (0 == j ? 0 : (finalPointPosTable[j] < (int32_t)i));

			m_OutsidePointIndex[i][j][0] = (finalPointPosTable[j] < (int32_t)i);
		}

		m_InsidePointIndex[i][halfFactor + 1][0] = 0;
		m_OutsidePointIndex[i][halfFactor + 1][0] = 0;

		for (uint32_t k = 0; k <= halfFactor + 1; ++k)
		{
			m_InsidePointIndex[i][k][1] = (0 == k ? 0 : (m_InsidePointIndex[i][k - 1][0] + m_InsidePointIndex[i][k - 1][1]));

			m_OutsidePointIndex[i][k][1] = (0 == k ? 0 : (m_OutsidePointIndex[i][k - 1][0] + m_OutsidePointIndex[i][k - 1][1]));

			if (m_InsidePointIndex[i][k][0])
			{
				m_InsidePointIndex[i][m_InsidePointIndex[i][k][1]][2] = k;
			}

			if (m_OutsidePointIndex[i][k][0])
			{
				m_OutsidePointIndex[i][m_OutsidePointIndex[i][k][1]][2] = k;
			}
		}

		for (uint32_t m = halfFactor; m >= 0; --m)
		{
			if (m_InsidePointIndex[i][m][0])
			{
				m_InsidePointIndex[i][m_InsidePointIndex[i][halfFactor + 1][1] - m_InsidePointIndex[i][m + 1][1]][3] = m;
			}

			if (m_OutsidePointIndex[i][m][0])
			{
				m_OutsidePointIndex[i][m_OutsidePointIndex[i][halfFactor + 1][1] - m_OutsidePointIndex[i][m + 1][1]][3] = m;
			}
		}
	}
}

void Tessellator::CreateResource(uint32_t vertexCount, ID3D11Buffer *pVertexBuf)
{
	assert(g_Renderer && pVertexBuf);

	g_Renderer->CreateComputeShader(m_Res.EdgeFactor.Reference(), "Tessellator_EdgeFactor.hlsl", "Func_EdgeFactor");
	g_Renderer->CreateComputeShader(m_Res.ScatterVertexTriID_IndexID.Reference(), "Tessellator_ScatterID.hlsl", "Func_ScatterVertexTriID_IndexID");
	g_Renderer->CreateComputeShader(m_Res.ScatterIndexTriID_IndexID.Reference(), "Tessellator_ScatterID.hlsl", "Func_ScatterIndexTriID_IndexID");
}
