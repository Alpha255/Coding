#include "CSVWriter.h"

#include <assert.h>

CSVWriter::CSVWriter(const char *outFilePath)
{
	m_FileStream = std::ofstream(outFilePath, std::ios::out);
	assert(m_FileStream.good());
}

void CSVWriter::WriteStats()
{

}

void CSVWriter::WriteRow(const char *pRow)
{
	m_FileStream << pRow;
}

CSVWriter::~CSVWriter()
{
	m_FileStream.close();
}