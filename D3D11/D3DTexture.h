#pragma once

#include "D3DResource.h"

class D3DTexture : public D3DResource<struct ID3D11ShaderResourceView>
{
public:
	enum eType
	{
		e1D,
		e2D,
		e3D,
		eCubemap,
		eTypeCount
	};

	D3DTexture() = default;
	~D3DTexture() = default;

	void CreateFromDDS(const char *pFileName);
protected:
	eType m_Type = eTypeCount;
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;
	uint32_t m_MipLevel = 1U;
private:
};

class D3DTexture1D : public D3DTexture
{
public:
	D3DTexture1D()
	{
		m_Type = e1D;
	}
protected:
private:
};

class D3DTexture2D : public D3DTexture
{
public:
	D3DTexture2D()
	{
		m_Type = e2D;
	}
protected:
private:
};

class D3DTexture3D : public D3DTexture
{
public:
	D3DTexture3D()
	{
		m_Type = e3D;
	}
protected:
private:
};

class D3DTextureCube : public D3DTexture
{
public:
	D3DTextureCube()
	{
		m_Type = eCubemap;
	}
protected:
private:
};