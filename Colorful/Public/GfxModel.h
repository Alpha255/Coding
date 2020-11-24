#pragma once

#include "Colorful/Public/GfxResource.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(Model)
class EXPORT_API Model
{
public:
	Model(const ModelDesc& desc);

	static Model createCube(float32_t length);
protected:
private:
};

NAMESPACE_END(Gfx)
