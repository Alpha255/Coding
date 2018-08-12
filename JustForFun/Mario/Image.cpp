#include "Image.h"
#include "System.h"

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
		".jpg"
	};

	std::string filePath = System::ResourceFilePath(pFileName, System::eTexture);

	if (ext == s_Ext[eBmp])
	{
		CreateAsBmp(filePath.c_str());
	}
	else if (ext == s_Ext[ePng])
	{
		CreateAsPng(filePath.c_str());
	}
	else if (ext == s_Ext[eJpg])
	{
		CreateAsJpg(filePath.c_str());
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
