#pragma once

#include "Colorful/Public/GfxDefinitions.h"

class D3D11Buffer : public D3DObject<ID3D11Buffer>
{
};
using D3D11BufferPtr = D3D11Buffer*;

class D3D11BufferPool : public LazySingleton<D3D11BufferPool>
{
	lazySingletonDeclare(D3D11BufferPool);
};
