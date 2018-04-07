#pragma once

#include "Common.h"

class Image
{
public:
	enum eType
	{
		eBmp,
		ePng,
		eJpg,
		eTypeCount
	};

	Image() = default;
	~Image()
	{
		SafeDeleteArray(m_pData);
		SafeDelete(m_pBitmapInfo);
	}

	void Create(const char *pFileName);

	inline uint32_t Width() const 
	{
		assert(m_pData && m_Width);
		return m_Width;
	}

	inline uint32_t Height() const
	{
		assert(m_pData && m_Height);
		return m_Height;
	}

	inline const byte *Data() const
	{
		assert(m_pData);
		return m_pData;
	}

	inline const LPBITMAPINFO BitmapInfo() const
	{
		assert(m_pBitmapInfo);
		return m_pBitmapInfo;
	}
protected:
	void CreateAsBmp(const char *pFilePath);
	void CreateAsPng(const char *pFilePath);
	void CreateAsJpg(const char *pFilePath);
private:
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	byte *m_pData = nullptr;
	LPBITMAPINFO m_pBitmapInfo = nullptr;
};
