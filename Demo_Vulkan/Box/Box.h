#pragma once

#include "RenderApp.h"

class AppBox : public RenderApp
{
public:
	AppBox() = default;
	~AppBox() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
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
