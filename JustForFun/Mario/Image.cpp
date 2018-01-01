#include "Image.h"

#include <string>
#include <fstream>
#include "Utility.h"

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

	std::string filePath = Utility::ResourceFilePath(pFileName, Utility::eTexture);

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
	std::ifstream bmpFile(pFilePath, std::ios::in);
	if (bmpFile.good())
	{
		BITMAPFILEHEADER fileHeader{};
		bmpFile.read((char *)&fileHeader, sizeof(BITMAPFILEHEADER));
		assert(fileHeader.bfType == 0x4d42); /// "BM"
		///assert((sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256) == fileHeader.bfOffBits);

		BITMAPINFOHEADER infoHeader{};
		bmpFile.read((char*)&infoHeader, sizeof(BITMAPINFOHEADER));

		RGBQUAD rgb[256]{}; /// ???
		bmpFile.read((char *)rgb, sizeof(RGBQUAD) * 256); /// ???

		LPBITMAPINFO lpBmpInfo = (LPBITMAPINFO)new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256]();
		memcpy(&lpBmpInfo->bmiHeader, &infoHeader, sizeof(BITMAPINFOHEADER));
		memcpy(lpBmpInfo->bmiColors, rgb, sizeof(RGBQUAD) * 256);

		bmpFile.seekg(fileHeader.bfOffBits, std::ios::beg);
		uint32_t bytesPerLine = (infoHeader.biWidth * (infoHeader.biBitCount / 8) + 3) / 4 * 4; 
		size_t dataSize = bytesPerLine * abs(infoHeader.biHeight);
		m_pData = new byte[dataSize]();
		bmpFile.read((char *)m_pData, dataSize);

		bmpFile.close();

		delete lpBmpInfo;
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
