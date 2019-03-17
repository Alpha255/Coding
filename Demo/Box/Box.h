#pragma once

#include "Colorful/Public/IRenderApplication.h"

class Box : public IRenderApplication
{
public:
	Box() = default;
	~Box() = default;

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
