
#pragma once

#include "D3DApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppMultithreadedRendering : public D3DApp
{
public:
	AppMultithreadedRendering() = default;
	~AppMultithreadedRendering() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	enum eRenderingMode
	{
		eSingleThread_IMContext,             /// Traditional rendering, one thread, immediate device context
		eSingleThread_DefContext_PerScene,   /// One thread, multiple deferred device contexts, one per scene 
		eSingleThread_DefContext_PerChunk,   /// One thread, multiple deferred device contexts, one per physical processor
		eMultiThread_DefContext_PerScene,    /// Multiple threads, one per scene, each with one deferred device context 
		eMultiThread_DefContext_PerChunk,    /// Multiple threads, one per physical processor, each with one deferred device context
	};
private:
};
