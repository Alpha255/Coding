#pragma once

#include "Colorful/Public/GfxEngine.h"

namespace AssetTool
{
	class AssetDatabase;
}

class GfxModel
{
public:
	struct GfxVertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec3 Tangent;
		Vec2 UV;
	};

	void load(const std::string& modelName);
protected:
	friend class AssetTool::AssetDatabase;
private:
	GfxGpuBufferPtr m_VertexBuffer = nullptr;
	GfxGpuBufferPtr m_IndexBuffer = nullptr;
	AABB m_BoundingBox;
};
