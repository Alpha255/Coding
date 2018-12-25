//#pragma once
//
//#include "D3DBuffer.h"
//#include "D3DView.h"
//#include "D3DShader.h"
//#include "D3DState.h"
//#include "D3DGeometry.h"
//
//class D3DSkyBox : public NoneCopyable
//{
//public:
//	D3DSkyBox() = default;
//	~D3DSkyBox() = default;
//
//	void Create(const char *pCubemapName, float skySphereRadius);
//
//	void Draw(const class Camera &cam);
//protected:
//private:
//	D3DVertexShader m_VertexShader;
//	D3DPixelShader m_PixelShader;
//
//	D3DBuffer m_CBufferVS;
//
//	D3DDepthStencilState m_DepthLessEqual;
//
//	D3DShaderResourceView m_Cubemap;
//
//	Geometry::Mesh m_SphereMesh;
//
//	bool m_Inited = false;
//};
