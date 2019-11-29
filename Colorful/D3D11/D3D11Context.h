#pragma once

#include "Colorful/Public/RAsset.h"

class d3d11Context final : public d3dObject<ID3D11DeviceContext>, public rContext
{

};
typedef std::shared_ptr<d3d11Context> d3d11ContextPtr;

class dxgiFactory : public d3dObject<IDXGIFactory>
{
};
typedef std::shared_ptr<dxgiFactory> dxgiFactoryPtr;

class dxgiFactory1 : public d3dObject<IDXGIFactory1>
{
};
typedef std::shared_ptr<dxgiFactory1> dxgiFactory1Ptr;

class dxgiFactory2 : public d3dObject<IDXGIFactory2>
{
};
typedef std::shared_ptr<dxgiFactory2> dxgiFactory2Ptr;

class dxgiFactory3 : public d3dObject<IDXGIFactory3>
{
};
typedef std::shared_ptr<dxgiFactory3> dxgiFactory3Ptr;

class dxgiFactory4 : public d3dObject<IDXGIFactory4>
{
};
typedef std::shared_ptr<dxgiFactory4> dxgiFactory4Ptr;

class dxgiFactory5 : public d3dObject<IDXGIFactory5>
{
};
typedef std::shared_ptr<dxgiFactory5> dxgiFactory5Ptr;

class dxgiFactory6 : public d3dObject<IDXGIFactory6>
{
};
typedef std::shared_ptr<dxgiFactory6> dxgiFactory6Ptr;

class dxgiFactory7 : public d3dObject<IDXGIFactory7>
{
};
typedef std::shared_ptr<dxgiFactory7> dxgiFactory7Ptr;

class dxgiAdapter : public d3dObject<IDXGIAdapter>
{
};
typedef std::shared_ptr<dxgiAdapter> dxgiAdapterPtr;

class dxgiAdapter1 : public d3dObject<IDXGIAdapter1>
{
};
typedef std::shared_ptr<dxgiAdapter1> dxgiAdapter1Ptr;

class dxgiAdapter2 : public d3dObject<IDXGIAdapter2>
{
};
typedef std::shared_ptr<dxgiAdapter2> dxgiAdapter2Ptr;

class dxgiAdapter3 : public d3dObject<IDXGIAdapter3>
{
};
typedef std::shared_ptr<dxgiAdapter3> dxgiAdapter3Ptr;

class dxgiAdapter4 : public d3dObject<IDXGIAdapter4>
{
};
typedef std::shared_ptr<dxgiAdapter4> dxgiAdapter4Ptr;

class dxgiOutput : public d3dObject<IDXGIOutput>
{
};
typedef std::shared_ptr<dxgiOutput> dxgiOutputPtr;

class dxgiOutput1 : public d3dObject<IDXGIOutput1>
{
};
typedef std::shared_ptr<dxgiOutput1> dxgiOutput1Ptr;

class dxgiOutput2 : public d3dObject<IDXGIOutput2>
{
};
typedef std::shared_ptr<dxgiOutput2> dxgiOutput2Ptr;

class dxgiOutput3 : public d3dObject<IDXGIOutput3>
{
};
typedef std::shared_ptr<dxgiOutput3> dxgiOutput3Ptr;

class dxgiOutput4 : public d3dObject<IDXGIOutput4>
{
};
typedef std::shared_ptr<dxgiOutput4> dxgiOutput4Ptr;

class dxgiOutput5 : public d3dObject<IDXGIOutput5>
{
};
typedef std::shared_ptr<dxgiOutput5> dxgiOutput5Ptr;

class dxgiOutput6 : public d3dObject<IDXGIOutput6>
{
};
typedef std::shared_ptr<dxgiOutput6> dxgiOutput6Ptr;
