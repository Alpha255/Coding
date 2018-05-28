#pragma once

#include "Common.h"

#include <d3d11.h>

template<typename T> class D3DObject
{
public:
	D3DObject() = default;
	~D3DObject() = default;

	inline bool IsValid() const
	{
		return m_Object.get() != nullptr;
	}

	inline T *Get() const
	{
		return m_Object.get();
	}

	inline void Reset()
	{
		if (m_Object)
		{
			m_Object.reset();
		}
	}

	inline D3DObject(const D3DObject &other)
	{
		if (m_Object != other.m_Object)
		{
			m_Object = other.m_Object;
		}
	}

	///inline void operator=(const D3DObject &other)
	///{
	///	if (m_Object != other.m_Object)
	///	{
	///		m_Object = other.m_Object;
	///	}
	///}

	inline T * const operator->() const
	{
		assert(IsValid());
		return m_Object.get();
	}

	inline bool operator==(const D3DObject &other) const
	{
		if (IsValid() && other.IsValid())
		{
			return (void*)m_Object.get() == (void*)other.Get();
		}
		else if (!IsValid() && !other.IsValid())
		{
			return true;
		}
		
		return false;
	}

	inline bool operator!=(const D3DObject &other) const
	{
		return !(*this == other);
	}

	inline void MakeObject(T *pObject)
	{
		if (pObject)
		{
			m_Object.reset(pObject, std::function<void(T *)>([](T *pObject){ SafeRelease(pObject) }));
		}
	}
protected:
	std::shared_ptr<T> m_Object;
private:
};

class D3DDevice : public D3DObject<ID3D11Device> {};
class D3DContext : public D3DObject<ID3D11DeviceContext> {};
class DXGIFactory : public D3DObject<IDXGIFactory> {};
class D3DResource : public D3DObject<ID3D11Resource> {};

class D3DSwapChain : public D3DObject<IDXGISwapChain> 
{
public:
	void Create(HWND hWnd, uint32_t width, uint32_t height, bool bWindowed);
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
		HRCheck(m_Object->Present(m_bVSync ? 1U : 0U, 0U));
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

class D3DViewport : public D3D11_VIEWPORT
{
public:
	D3DViewport()
	{
		memset(this, 0, sizeof(D3D11_VIEWPORT));
	}

	D3DViewport(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f)
	{
		TopLeftX = topLeftX;
		TopLeftY = topLeftY;
		Width = width;
		Height = height;
		MinDepth = minDepth;
		MaxDepth = maxDepth;
	}
};

class D3DRect : public D3D11_RECT
{
public:
	D3DRect()
	{
		memset(this, 0, sizeof(D3D11_RECT));
	}

	D3DRect(uint32_t l, uint32_t t, uint32_t r, uint32_t b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
};
