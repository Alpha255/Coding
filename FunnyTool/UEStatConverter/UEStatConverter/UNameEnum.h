#pragma once

#define RegisterName(num, name) NAME_##name = num,
enum eUNameEnum
{
#include "UnrealNames.inl"
	eUNameMaxHardcodedNameIndex
};
#undef RegisterName
