#pragma once
//
//#include "D3DGraphic.h"
//
//class SkyBox
//{
//public:
//	SkyBox() = default;
//	~SkyBox() = default;
//
//	void Create(const char *pCubemapName, float skySphereRadius);
//
//	void Draw(const class Camera &cam);
//
//	inline const Ref<ID3D11ShaderResourceView> &GetCubemap() const
//	{
//		assert(m_Cubemap.Valid());
//		return m_Cubemap;
//	}
//protected:
//	SkyBox(const SkyBox &) = default;
//	SkyBox & operator=(const SkyBox &) = default;
//
//	void RestorState();
//
//	struct ConstantsBufferVS
//	{
//		Matrix WVP;
//	};
//private:
//	uint32_t m_IndexCount = 0U;
//
//	Ref<ID3D11InputLayout> m_Layout;
//
//	Ref<ID3D11Buffer> m_VertexBuffer;
//	Ref<ID3D11Buffer> m_IndexBuffer;
//
//	Ref<ID3D11VertexShader> m_VertexShader;
//	Ref<ID3D11PixelShader> m_PixelShader;
//
//	Ref<ID3D11Buffer> m_ConstansBuffer;
//
//	Ref<ID3D11SamplerState> m_Sampler;
//
//	Ref<ID3D11RasterizerState> m_NoBackFaceCulling;
//
//	Ref<ID3D11DepthStencilState> m_DepthLess_Equal;
//
//	Ref<ID3D11ShaderResourceView> m_Cubemap;
//};
