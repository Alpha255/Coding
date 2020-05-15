#pragma once

#include "Colorful/Public/GfxEngine.h"

class DXGIFactory : public D3DObject<IDXGIFactory>
{
};

class DXGIFactory1 : public D3DObject<IDXGIFactory1>
{
};

class DXGIFactory2 : public D3DObject<IDXGIFactory2>
{
};

class DXGIFactory3 : public D3DObject<IDXGIFactory3>
{
};

class DXGIFactory4 : public D3DObject<IDXGIFactory4>
{
};

class DXGIFactory5 : public D3DObject<IDXGIFactory5>
{
};

class DXGIFactory6 : public D3DObject<IDXGIFactory6>
{
};

class DXGIFactory7 : public D3DObject<IDXGIFactory7>
{
};

class DXGIAdapter : public D3DObject<IDXGIAdapter>
{
};

class DXGIAdapter1 : public D3DObject<IDXGIAdapter1>
{
};

class DXGIAdapter2 : public D3DObject<IDXGIAdapter2>
{
};

class DXGIAdapter3 : public D3DObject<IDXGIAdapter3>
{
};

class DXGIAdapter4 : public D3DObject<IDXGIAdapter4>
{
};

class DXGIOutput : public D3DObject<IDXGIOutput>
{
};

class DXGIOutput1 : public D3DObject<IDXGIOutput1>
{
};

class DXGIOutput2 : public D3DObject<IDXGIOutput2>
{
};

class DXGIOutput3 : public D3DObject<IDXGIOutput3>
{
};

class DXGIOutput4 : public D3DObject<IDXGIOutput4>
{
};

class DXGIOutput5 : public D3DObject<IDXGIOutput5>
{
};

class DXGIOutput6 : public D3DObject<IDXGIOutput6>
{
};

template<class Factory>
Factory CreateDXGIFactory(uint32_t flags)
{
	Factory::Type* factory = nullptr;
	GfxVerifyD3D(CreateDXGIFactory2(flags, __uuidof(Factory::Type), reinterpret_cast<void**>(&factory)));

	Factory result;
	result.reset(factory);

	return result;
}
