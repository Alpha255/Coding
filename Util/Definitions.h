#pragma once

#define UsingD3D11
///#define UsingVulkan

#if defined(UsingD3D11)
	#include "D3D11/D3D11Engine.h"
	typedef D3D11Engine REngine;
	typedef D3D11VertexShader RVertexShader;
	typedef D3D11PixelShader RPixelShader;
	typedef D3D11Buffer RBuffer;
	typedef D3D11ShaderResourceView RShaderResourceView;
	typedef D3D11Viewport RViewport;
	typedef D3D11InputLayout RInputLayout;
	typedef D3D11BlendState RBlendState;
	typedef D3D11Texture2D RTexture2D;
	typedef D3D11StaticState RStaticState;
#elif defined(UsingVulkan)
	typedef class VulkanEngine REngine;
	typedef class VulkanVertexShader RVertexShader;
	typedef class VulkanPixelShader RPixelShader;
	typedef class VulkanBuffer RBuffer;
	typedef class VulkanShaderResourceView RShaderResourceView;
	typedef class VulkanViewport RViewport;
#endif