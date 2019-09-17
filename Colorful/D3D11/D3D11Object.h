#pragma once

#include "D3D11Types.h"

template<typename T> class D3DObject
{
public:
	D3DObject() = default;
	virtual ~D3DObject() = default;

	inline bool IsValid() const
	{
		return m_Object.get() != nullptr;
	}

	inline T *Get() const
	{
		return m_Object.get();
	}

	inline void Reset(T *pObject = nullptr)
	{
		if (pObject)
		{
			m_Object.reset(pObject, std::function<void(T *)>([](T * pObject) { pObject->Release(); }));
		}
		else
		{
			if (m_Object)
			{
				m_Object.reset();
				m_Object = nullptr;
			}
		}
	}

	inline D3DObject(const D3DObject &other)
	{
		if (m_Object != other.m_Object)
		{
			m_Object = other.m_Object;
		}
	}

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

	inline bool operator==(const T *other) const
	{
		return (void*)m_Object.get() == (void*)other;
	}

	inline bool operator!=(const D3DObject &other) const
	{
		return !(*this == other);
	}

	inline bool operator!=(const T *other) const
	{
		return (void*)m_Object.get() != (void*)other;
	}
protected:
	std::shared_ptr<T> m_Object;
private:
};

class D3D11Viewport : public D3D11_VIEWPORT
{
public:
	inline D3D11Viewport()
	{
		memset(this, 0, sizeof(D3D11_VIEWPORT));
	}

	inline D3D11Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f)
	{
		TopLeftX = topLeftX;
		TopLeftY = topLeftY;
		Width = width;
		Height = height;
		MinDepth = minDepth;
		MaxDepth = maxDepth;
	}
};

class D3D11Rect : public D3D11_RECT
{
public:
	inline D3D11Rect()
	{
		memset(this, 0, sizeof(D3D11_RECT));
	}

	inline D3D11Rect(::RECT &rect)
	{
		memcpy(this, &rect, sizeof(::RECT));
	}

	inline D3D11Rect(uint32_t l, uint32_t t, uint32_t r, uint32_t b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
};

class D3D11Device : public D3DObject<ID3D11Device> {};
class DXGIFactory : public D3DObject<IDXGIFactory> {};
class D3D11Resource : public D3DObject<ID3D11Resource> {};
class D3D11Blob : public D3DObject<ID3DBlob> {};

class D3D11CommandList
{
public:
	inline bool IsValid() const
	{
		return m_CommandList != nullptr;
	}

	ID3D11CommandList *Get()
	{
		return m_CommandList;
	}

	ID3D11CommandList **operator &()
	{
		return &m_CommandList;
	}

	inline void Release()
	{
		SafeRelease(m_CommandList);
	}

	~D3D11CommandList()
	{
		Release();
	}
protected:
private:
	ID3D11CommandList *m_CommandList = nullptr;
};
