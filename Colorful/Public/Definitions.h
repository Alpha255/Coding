#pragma once

#include "Base/Resource.h"

#define UsingD3D11
///#define UsingVulkan

#if defined(UsingD3D11)
	#include "D3D11/D3D11Engine.h"
	#define IconID IconDirectX 
	typedef D3D11Engine REngine;
	typedef D3D11VertexShader RVertexShader;
	typedef D3D11PixelShader RPixelShader;
	typedef D3D11Buffer RBuffer;
	typedef D3D11ShaderResourceView RShaderResourceView;
	typedef D3D11RenderTargetView RRenderTargetView;
	typedef D3D11Viewport RViewport;
	typedef D3D11InputLayout RInputLayout;
	typedef D3D11BlendState RBlendState;
	typedef D3D11Texture2D RTexture2D;
	typedef D3D11StaticState RStaticState;
	typedef D3D11SubResourceData RSubResourceData;
	typedef D3D11DepthStencilState RDepthStencilState;
	typedef D3D11RasterizerState RRasterizerState;
	typedef D3D11DepthStencilView RDepthStencilView;
	typedef D3D11Event REvent;
	typedef D3D11CommandList RCommandList;
	typedef D3D11Context RContext;
#elif defined(UsingVulkan)
	#include "Vulkan/VulkanEngine.h"
	#define IconID IconVulkan 
	typedef VulkanEngine REngine;
	typedef VulkanVertexShader RVertexShader;
	typedef VulkanPixelShader RPixelShader;
	typedef VulkanBuffer RBuffer;
	typedef VulkanShaderResourceView RShaderResourceView;
	typedef VulkanViewport RViewport;
	typedef VulkanInputLayout RInputLayout;
	typedef VulkanBlendState RBlendState;
	typedef VulkanTexture2D RTexture2D;
	typedef VulkanStaticState RStaticState;
	typedef VulkanSubResourceData RSubResourceData;
#else
	#error "Unknown Render Interface"
#endif

#define GpuMarkerBegin(Name) REvent GpuMarker; GpuMarker.Begin(Name);
#define GpuMarkerEnd() GpuMarker.End();