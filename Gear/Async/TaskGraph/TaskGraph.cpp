#include "Gear/Async/TaskGraph/TaskGraph.h"

NAMESPACE_START(Gear)

void TaskGraph::initialize()
{
	std::shared_ptr<byte8_t> buffer = nullptr;
	ulong32_t bufferSize = 0u;

	uint32_t coreCount = 0u;
	uint32_t logicalProcessorCount = 0u;

	if (!::GetLogicalProcessorInformationEx(
		LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll,
		reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.get()),
		&bufferSize))
	{
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			buffer.reset(new byte8_t[bufferSize]());
			if (::GetLogicalProcessorInformationEx(
				LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll,
				reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.get()),
				&bufferSize))
			{
				for (uint32_t i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX); ++i)
				{
					auto const &sysProcessorInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.get())[i];
					if (sysProcessorInfo.Relationship == LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore)
					{
						++coreCount;
						for (uint32_t group = 0u; group < sysProcessorInfo.Processor.GroupCount; ++group)
						{
							logicalProcessorCount += static_cast<uint32_t>(std::bitset<sizeof(KAFFINITY)>(sysProcessorInfo.Processor.GroupMask[group].Mask).count());
						}
					}
				}
			}
		}
	}
}

void TaskGraph::finalize()
{

}

NAMESPACE_END(Gear)