#include "StatFileConverter.h"
#include "CommandLine.h"
#include "Utility.h"

#include <thread>
#include <iostream>

static const char *s_StatList[] =
{
	"STAT_ParticleComputeTickTime",
	"STAT_ParticleRenderingTime",
	"STAT_ParticleDrawCalls",
	"STAT_GTParticleData",
	"STAT_PopcornFX_PopcornFXUpdateTime",
	"STAT_PopcornFX_GDMETime",
	"STAT_PopcornFX_DrawCallsCount",
	"STAT_PopcornFX_GTParticleData",
	"STAT_FrameTime",
	"Stat_GPU_Total"
};

static const char *s_Postfix[] =
{
	"Simulation",
	"Rendering",
	"Drawcall",
	"ParticleMemory",
	"Simulation",
	"Rendering",
	"Drawcall",
	"ParticleMemory",
	"FrameTime",
	"GPUTime"
};

static std::string s_UEFrontendDir;

void StatFileConverter::DoConvert()
{
	s_UEFrontendDir = std::string(CommandLine::GetUERootDir());
	s_UEFrontendDir += "\\Engine\\Binaries\\Win64\\UnrealFrontend.exe";

	Utility::GetFileListFromDirectory(m_FileList, CommandLine::GetInFileDir(), ".ue4stats");

	uint32_t threadNum = std::thread::hardware_concurrency() - 1; /// main thread
	uint32_t totalFileNum = (uint32_t)m_FileList.size();
	uint32_t fileNumPerThread = totalFileNum / threadNum;

	for (uint32_t i = 0U; i < threadNum; ++i)
	{
		uint32_t start = i * fileNumPerThread;
		uint32_t end = start + fileNumPerThread;
		end = (i == (threadNum - 1) ? (totalFileNum - 1) : end);

		std::thread task(&StatFileConverter::DoTask, this, start, end);
		task.join();
	}
}

void StatFileConverter::DoTask(uint32_t startIndex, uint32_t endIndex)
{
	for (uint32_t i = startIndex; i < endIndex; ++i)
	{
		std::string outFilePrefix(m_FileList.at(i).begin(), m_FileList.at(i).begin() + m_FileList.at(i).rfind('.') + 1);

		for (uint32_t j = 0U; j < ARRAYSIZE(s_StatList); ++j)
		{
			std::string cmd = s_UEFrontendDir;
			cmd += " -RUN=Convert -INFILE=";
			cmd += m_FileList.at(i);

			std::string outFile(outFilePrefix);
			outFile += s_Postfix[j];
			outFile += ".csv";

			cmd += " -OUTFILE=";
			cmd += outFile;
			cmd += " -STATLIST=";
			cmd += s_StatList[j];
			///cmd += "\n";
			///OutputDebugStringA(cmd.c_str());

			::STARTUPINFOA si = {};
			si.cb = sizeof(STARTUPINFOA);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;

			::PROCESS_INFORMATION pi = {};
			if (!::CreateProcessA(nullptr, (LPSTR)cmd.c_str(), nullptr, nullptr, false, 0U, nullptr, nullptr, &si, &pi))
			{
				OutputDebugStringA("Failed to create process!!!\n");
			}

			///::WaitForSingleObject(pi.hProcess, INFINITE);
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}

		UpdateProgress(i);
	}
}

void StatFileConverter::UpdateProgress(uint32_t index)
{
	m_Mutex.lock();
	m_ProcessedFileNum += 1;
	double percent = (double)m_ProcessedFileNum / (double)m_FileList.size() * 100.0f;
	const char *pCurFile = m_FileList.at(index).c_str() + m_FileList.at(index).rfind('\\') + 1;
	std::cout << "Processing file: " << pCurFile << " " << percent << "%" << std::endl;
	m_Mutex.unlock();
}
