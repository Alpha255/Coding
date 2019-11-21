#pragma once

#include "Colorful/Public/RAsset.h"

class d3d11Context final : public d3dObject<ID3D11DeviceContext>, public rContext
{

};
typedef std::shared_ptr<d3d11Context> d3d11ContextPtr;

class d3d11DxgiFactory : public d3dObject<IDXGIFactory>
{
};
typedef std::shared_ptr<d3d11DxgiFactory> d3d11DxgiFactoryPtr;

class d3d11DxgiFactory1 : public d3dObject<IDXGIFactory1>
{
};
typedef std::shared_ptr<d3d11DxgiFactory1> d3d11DxgiFactory1Ptr;

class d3d11DxgiFactory2 : public d3dObject<IDXGIFactory2>
{
};
typedef std::shared_ptr<d3d11DxgiFactory2> d3d11DxgiFactory2Ptr;

class d3d11DxgiAdapter : public d3dObject<IDXGIAdapter>
{
};
typedef std::shared_ptr<d3d11DxgiAdapter> d3d11DxgiAdapterPtr;

class d3d11DxgiAdapter1 : public d3dObject<IDXGIAdapter1>
{
};
typedef std::shared_ptr<d3d11DxgiAdapter1> d3d11DxgiAdapterPtr1;

class d3d11DxgiAdapter2 : public d3dObject<IDXGIAdapter2>
{
};
typedef std::shared_ptr<d3d11DxgiAdapter2> d3d11DxgiAdapterPtr2;
