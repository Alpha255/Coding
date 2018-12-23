#pragma once

#if defined(UsingD3D11)
	typedef class D3DEngine REngine;
	typedef class D3DVertexShader RVertexShader;
	typedef class D3DPixelShader RPixelShader;
	typedef class D3DBuffer RBuffer;
	typedef class D3DShaderResourceView RShaderResourceView;
	typedef class D3DViewport RViewport;
#elif defined(UsingVulkan)
#endif
