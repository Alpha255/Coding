#pragma once

#include "File.h"

namespaceStart(gear)

class fileIO : public file
{
public:
	enum eRead
	{
		eText,
		eBinary
	};

	fileIO() = default;
	
	fileIO(const std::string &filePath)
		: file(filePath)
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
	std::shared_ptr<byte> m_Data;
};

namespaceEnd(gear)
