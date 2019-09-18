#pragma once

#include "D3D11Object.h"

class D3D11Event : public D3DObject<ID3DUserDefinedAnnotation>
{
public:
	D3D11Event(class D3D11Context *pContext = nullptr);
	~D3D11Event() = default;

	void Begin(const std::string &info);
	void End();
protected:
private:
};
