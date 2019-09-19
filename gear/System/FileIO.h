#pragma once

#include "File.h"

namespaceStart(gear)

class fileIO
{
public:
	enum eRead
	{
		eText,
		eBinary
	};

	fileIO() = default;
	
	fileIO(const std::string &filePath)
		: m_File(std::make_unique<file>(filePath))
	{
	}

	void Read(eRead readType = eText);

	std::shared_ptr<byte> getData() const
	{
		assert(m_Data);
		return m_Data;
	}
protected:
private:
	std::unique_ptr<file> m_File;
	std::shared_ptr<byte> m_Data;
};

namespaceEnd(gear)
