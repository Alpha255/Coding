#pragma once

#include <fstream>

class CSVWriter
{
public:
	CSVWriter(const char *outFilePath);
	~CSVWriter();

	void WriteStats();
	void WriteRow(const char *pRow);
protected:
private:
	std::ofstream m_FileStream;
};
