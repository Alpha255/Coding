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
	}

	void Create(const char *pFileName);

	inline uint32_t Width() 
	{
		assert(m_pData && m_Width);
		return m_Width;
	}

	inline uint32_t Height()
	{
		assert(m_pData && m_Height);
		return m_Height;
	}

	inline const byte *Data() const
	{
		assert(m_pData);
		return m_pData;
	}
protected:
	void CreateAsBmp(const char *pFilePath);
	void CreateAsPng(const char *pFilePath);
	void CreateAsJpg(const char *pFilePath);
private:
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;

	byte *m_pData = nullptr;
};
