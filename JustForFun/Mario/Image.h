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
	~Image() = default;

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
		return m_pData.get();
	}

	inline const LPBITMAPINFO BitmapInfo() const
	{
		return (LPBITMAPINFO)m_BitmapInfo.get();
	}
protected:
	void CreateAsBmp(const char *pFilePath);
	void CreateAsPng(const char *pFilePath);
	void CreateAsJpg(const char *pFilePath);
private:
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	std::shared_ptr<byte> m_pData = nullptr;
	std::shared_ptr<BITMAPINFO> m_BitmapInfo = nullptr;
};
