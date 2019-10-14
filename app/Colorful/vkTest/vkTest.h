#pragma once

#include "gear/System/Application.h"

class vkTest : public application
{
public:
	void postInitialize() override;

	void finalize() override;

	void resizeWindow() override;

	void renterToWindow() override;
protected:
private:
};