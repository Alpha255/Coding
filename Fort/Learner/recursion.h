#pragma once

#include "node.h"

void Move(uint32_t index, const char &from, const char &to)
{
	std::cout << "Move " << index << " from " << from << " to " << to << std::endl;
	char info[MAX_PATH] = {};
	sprintf_s(info, "Move %d from %c to %c\n", index, from, to);
	::OutputDebugStringA(info);
}

void Hanoi(uint32_t count, const char &left, const char &middle, const char &right)
{
	if (1u == count)
	{
		Move(count, left, right);
	}
	else
	{
		Hanoi(count - 1u, left, right, middle);
		Move(count, left, right);
		Hanoi(count - 1u, middle, left, right);
	}
}

struct Movement
{
	uint32_t Count = 0U;
	char Left;
	char Middle;
	char Right;
};

void _Hanoi_(uint32_t count, const char &left, const char &middle, const char &right)
{
	std::stack<Movement> movements;
	Movement movement{ count, left, middle, right };
	while (!(movement.Count == 0u && movements.empty()))
	{
		Movement nextMovement = movement;
		while (nextMovement.Count != 0u)
		{
			movements.push(nextMovement);
			--nextMovement.Count;
			std::swap(nextMovement.Middle, nextMovement.Right);
		}

		movement = movements.top();
		movements.pop();

		Move(movement.Count, movement.Left, movement.Right);
		--movement.Count;
		std::swap(movement.Left, movement.Middle);
	}
}
