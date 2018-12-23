#pragma once

#include "Common.h"

class IRenderEngine : public NoneCopyable
{
public:
	IRenderEngine() = default;
	virtual ~IRenderEngine() = default;

	virtual void Initialize(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	virtual void Flush() = 0;
protected:
private:
};
