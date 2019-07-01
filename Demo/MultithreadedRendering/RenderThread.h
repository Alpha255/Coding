#pragma once

#include "Base/IThread.h"

class RenderThread : public IThread
{
public:
	void ThreadFunc() override;
protected:
private:
};
