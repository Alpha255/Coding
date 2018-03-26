#pragma once

#include "Common.h"

class D3DTexture
{
public:
	enum eTextureType
	{
		e1D,
		e2D,
		e3D,
		eCubemap,
		eTypeCount
	};

	D3DTexture() = default;
	~D3DTexture() = default;

	inline D3DTexture(const D3DTexture &srcTex)
	{
		assert(!IsValid());
		m_HWTexture = srcTex.m_HWTexture;
	}

	inline void operator=(const D3DTexture &srcTex)
	{
		assert(!IsValid());
		m_HWTexture = srcTex.m_HWTexture;
	}

	inline bool IsValid() const
	{
		return (m_HWTexture.get() != nullptr);
	}

	inline struct ID3D11ShaderResourceView *GetHWTexture() const
	{
		assert(IsValid());
		return m_HWTexture.get();
	}

	void Create(const char *pFileName);
protected:
private:
	std::shared_ptr<struct ID3D11ShaderResourceView> m_HWTexture;
};