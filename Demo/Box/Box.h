#pragma once

#include "Public/IRenderApplication.h"

class Box : public IRenderApplication
{
public:
	Box() = default;
	~Box() = default;

	void PrepareScene() override;
	void RenderScene() override;
protected:
	enum eSpecialEffect
	{
		eNone,
		eInversion,
		eGrayscale,
		eSharpen,
		eBlur,
		eEdgeDetection,
		eEffectCount
	};
private:
	int32_t m_Effect = eNone;
};
