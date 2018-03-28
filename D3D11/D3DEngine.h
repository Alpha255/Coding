#pragma once

#include "Common.h"
#include <iostream>

class D3DEngine
{
public:
	static const D3DEngine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance.reset(new D3DEngine());
		}
		return *s_Instance;
	}

	static void Destory()
	{
		s_Instance.reset(nullptr);
	}

	D3DEngine() = default;
	~D3DEngine() = default;
protected:
private:
	static std::unique_ptr<D3DEngine> s_Instance;
};
