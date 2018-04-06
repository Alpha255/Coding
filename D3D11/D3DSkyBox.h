#pragma once

#include "D3DBuffer.h"
#include "D3DView.h"
#include "D3DShader.h"
#include "D3DState.h"
#include "D3DGeometry.h"

class D3DSkyBox : public NoneCopyable
{
public:
	D3DSkyBox() = default;
	~D3DSkyBox() = default;

	void Create(const char *pCubemapName, float skySphereRadius);

	void Draw(const class Camera &cam);
protected:
	struct RenderResource
	{
		D3DInputLayout VertexLayout;

		D3DBuffer VertexBuffer;
		D3DBuffer IndexBuffer;
		D3DBuffer ConstantBufferVS;

		D3DVertexShader VertexShader;
		D3DPixelShader PixelShader;

		D3DDepthStencilState DepthLessEqual;

		D3DShaderResourceView Cubemap;
	};
private:
	RenderResource m_Resource;

	Geometry::Mesh m_Sphere;
};
