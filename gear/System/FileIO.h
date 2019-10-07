#pragma once

#include "File.h"

namespaceStart(gear)

class fileIO : public file
{
public:
	enum eReadAs
	{
		eText,
		eBinary
	};

	fileIO() = default;
	
	fileIO(const std::string &filePath)
		: file(filePath)
	{
	}

	std::shared_ptr<byte> getData(eReadAs readAs = eText)
	{
		if (!m_Data)
		{
			read(readAs);
		}

		return m_Data;
	}
protected:
	void read(eReadAs readAs = eText);
private:
	std::shared_ptr<byte> m_Data;
};

namespaceEnd(gear)
