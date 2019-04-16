#pragma once

#include "D3D11Object.h"

class D3D11SwapChain : public D3DObject<IDXGISwapChain>
{
public:
	void Create(HWND hWnd, uint32_t width, uint32_t height, bool bFullScreen);
	void Resize(uint32_t width, uint32_t height);

	inline void SetVSync(bool bVSync)
	{
		m_bVSync = bVSync;
	}

	inline void SetFullScreen(bool bFullScreen)
	{
		m_bFullScreen = bFullScreen;
	}

	inline void Flush()
	{
		assert(IsValid());

		/// DXGI_PRESENT 
		Check(m_Object->Present(m_bVSync ? 1U : 0U, 0U));
	}

	inline uint32_t Width() const
	{
		assert(IsValid());
		return m_Desc.BufferDesc.Width;
	}

	inline uint32_t Height() const
	{
		assert(IsValid());
		return m_Desc.BufferDesc.Height;
	}
protected:
	enum eConstant
	{
		eBufferCount = 2
	};
private:
	DXGI_SWAP_CHAIN_DESC m_Desc = {};
	bool m_bVSync = false;
	bool m_bFullScreen = false;
};
