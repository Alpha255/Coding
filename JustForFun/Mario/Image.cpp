#include "Image.h"
#include "System.h"
#include "DirectXTK/Src/DDS.h"

#include <string>
#include <fstream>

void Image::Create(const char *pFileName)
{
	assert(pFileName);

	std::string fileName(pFileName);
	std::string ext = fileName.substr(fileName.rfind('.'));
	assert(ext.length());

	static const char *const s_Ext[eTypeCount] = 
	{
		".bmp",
		".png",
		".jpg",
		".dds"
	};

	std::string filePath = System::ResourceFilePath(pFileName, System::eTexture);

	if (ext == s_Ext[eBmp])
	{
		m_ImageType = eBmp;
		CreateAsBmp(filePath.c_str());
	}
	else if (ext == s_Ext[ePng])
	{
		m_ImageType = ePng;
		CreateAsPng(filePath.c_str());
	}
	else if (ext == s_Ext[eJpg])
	{
		m_ImageType = eJpg;
		CreateAsJpg(filePath.c_str());
	}
	else if (ext == s_Ext[eDds])
	{
		m_ImageType = eDds;
		CreateAsDds(pFileName);
	}
	else
	{
		assert("Unsupport type!!!");
	}
}

void Image::CreateAsBmp(const char *pFilePath)
{
	std::ifstream bmpFile(pFilePath, std::ios::in | std::ios::binary);
	if (bmpFile.good())
	{
		::BITMAPFILEHEADER fileHeader{};
		bmpFile.read((char *)&fileHeader, sizeof(::BITMAPFILEHEADER));
		assert(fileHeader.bfType == 0x4d42); /// "BM"
		///assert((sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256) == fileHeader.bfOffBits);

		::BITMAPINFOHEADER infoHeader{};
		bmpFile.read((char*)&infoHeader, sizeof(::BITMAPINFOHEADER));

		m_Width = infoHeader.biWidth;
		m_Height = infoHeader.biHeight;

		assert(infoHeader.biBitCount == 8); /// Not for common use ???

		::RGBQUAD rgb[256]{}; /// ???
		bmpFile.read((char *)rgb, sizeof(::RGBQUAD) * 256); /// ???

		PBITMAPINFO pBitmapInfo = (PBITMAPINFO)new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256]();
		memcpy(&pBitmapInfo->bmiHeader, &infoHeader, sizeof(BITMAPINFOHEADER));
		memcpy(pBitmapInfo->bmiColors, rgb, sizeof(RGBQUAD) * 256);
		m_BitmapInfo.reset(pBitmapInfo);

		bmpFile.seekg(fileHeader.bfOffBits, std::ios::beg);
		uint32_t bytesPerLine = (infoHeader.biWidth * (infoHeader.biBitCount / 8) + 3) / 4 * 4; 
		size_t dataSize = bytesPerLine * ::abs(infoHeader.biHeight);

		byte *pData = new byte[dataSize]();
		bmpFile.read((char *)pData, dataSize);
		m_pData.reset(pData);

		bmpFile.close();
	}
	else
	{
		assert(0);
	}
}

void Image::CreateAsPng(const char * /*pFilePath*/)
{
	assert(0);
}

void Image::CreateAsJpg(const char * /*pFilePath*/)
{
	assert(0);
}

void Image::CreateAsDds(const char *pFileName)
{
	std::string filePath = System::ResourceFilePath(pFileName, System::eTexture);
	std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
	assert(fileStream.good());

	uint32_t dwMagicNumber = 0U;
	fileStream.read((char *)&dwMagicNumber, sizeof(uint32_t));
	assert(dwMagicNumber == DirectX::DDS_MAGIC);

	DirectX::DDS_HEADER header;
	fileStream.read((char *)&header, sizeof(DirectX::DDS_HEADER));
	assert(header.size == sizeof(DirectX::DDS_HEADER));
	assert(header.ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));

	m_Width = header.width;
	m_Height = header.height;

	m_ShaderResourceView.Create(pFileName);
}
