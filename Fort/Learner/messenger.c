#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

/// Compiled with VS2017

#define MAX_DIMENSION 100
#define INVALID_VALUE -1

int32_t *gWeightMatrix = NULL;  /// Store adjacent matrix, use array to store the adjacent matrix
int32_t *gWeights = NULL;       /// Store the weight from the capital to other city
bool *gVisited = NULL;          /// Store whether the city has been visited

uint32_t getDimension()
{
	assert(gWeightMatrix);

	uint32_t dimension = gWeightMatrix[0];
	assert(dimension <= MAX_DIMENSION);
	return dimension;
}

int32_t getWeight(uint32_t row, uint32_t col)
{
	assert(row < getDimension() && col < getDimension());

	if (row == col)
	{
		return 0;
	}

	if (row < col)
	{
		uint32_t temp = row;
		row = col;
		col = temp;
	}

	return gWeightMatrix[((row * (row - 1u)) >> 1u) + col + 1u];
}

void setWeight(uint32_t row, uint32_t col, int32_t weight)
{
	assert(row < getDimension() && col < getDimension());
	gWeightMatrix[((row * (row - 1u)) >> 1u) + col + 1u] = weight;
}

void initialize(uint32_t dimension)
{
	assert(dimension >= 1u && dimension <= MAX_DIMENSION);
	uint32_t length = ((dimension * (dimension - 1u)) >> 1u) + 1u;
	gWeightMatrix = (int32_t *)malloc(sizeof(int32_t) * length);
	assert(gWeightMatrix);
	memset(gWeightMatrix, INVALID_VALUE, sizeof(int32_t) * length);
	gWeightMatrix[0] = dimension;

	gWeights = (int32_t *)malloc(sizeof(int32_t) * dimension);
	assert(gWeights);
	memset(gWeights, INVALID_VALUE, sizeof(int32_t) * dimension);

	gVisited = (bool *)malloc(sizeof(bool) * dimension);
	assert(gVisited);
	memset(gVisited, false, sizeof(bool) * dimension);
}

void finalize()
{
	if (gWeightMatrix)
	{
		free(gWeightMatrix);
		gWeightMatrix = NULL;
	}

	if (gWeights)
	{
		free(gWeights);
		gWeights = NULL;
	}

	if (gVisited)
	{
		free(gVisited);
		gVisited = NULL;
	}
}

const char *getInput()
{
	static char line[_MAX_PATH];

	if (fgets(line, _MAX_PATH, stdin))
	{
		return line;
	}

	return NULL;
}

uint32_t getDimensionFromInput()
{
	uint32_t dimension = UINT32_MAX;
	while (dimension > MAX_DIMENSION)
	{
		printf_s("Please input the count of cities (less than %u): ", MAX_DIMENSION);
		const char *pInput = getInput();
		if (pInput)
		{
			bool bValid = true;
			for (uint32_t i = 0u; i < _MAX_PATH; ++i)
			{
				if (pInput[i] == '\0' || pInput[i] == '\n')
				{
					break;
				}
				bValid &= (isdigit(pInput[i]) != 0);
			}

			if (!bValid)
			{
				printf_s("Invalid input format, please input a positive number.\n\n");
				continue;
			}

			dimension = (uint32_t)atoi(pInput);
			if (dimension < 1u)
			{
				dimension = UINT32_MAX;
				printf_s("The count of cities must great than 1, please try again.\n\n");
			}
			else if (dimension > MAX_DIMENSION)
			{
				printf_s("The count of cities must less than %u, please try again.\n\n", MAX_DIMENSION);
			}
		}
		else
		{
			printf_s("Ivalid input, please try again.\n");
		}
	}

	return dimension;
}

void buildWeightMatrixFromInput()
{
	uint32_t dimension = getDimension();
	uint32_t row = 1u;
	while (row < dimension)
	{
		printf_s("Please input %u weights of city to city, \"x\" means no road between these two cities (separate by space):\n", row);
		const char *pInput = getInput();
		if (pInput)
		{
			uint32_t col = 0u;
			bool bValid = true;
			const char *pLine = pInput;
			while (*pLine != '\0' && *pLine != '\n')
			{
				if (col > dimension)
				{
					printf_s("The count of input elems must be %u, please try again.\n\n", row);
					break;
				}
				if (*pLine == 'x' || *pLine == 'X')
				{
					setWeight(row, col++, INVALID_VALUE);
					++pLine;
					continue;
				}
				if (*pLine == ' ')
				{
					++pLine;
					continue;
				}
				if (isdigit(*pLine))
				{
					int32_t weight = atoi(pLine);
					if (weight == INT32_MAX)
					{
						printf_s("Invalid input format/number, please try again.\n\n");
						bValid = false;
						break;
					}
					else
					{
						setWeight(row, col++, weight);
					}

					while (isdigit(*pLine))
					{
						++pLine;
					}
				}
				else
				{
					printf_s("Invalid input format, please try again.\n\n");
					bValid = false;
					break;
				}
			}

			if (bValid)
			{
				if (col == row)
				{
					++row;
				}
				else
				{
					printf_s("The count of input elems must be %u, please try again.\n\n", row);
				}
			}
		}
		else
		{
			printf_s("Ivalid input, please try again.\n\n");
		}
	}
}

void outputWeightMatrix()
{
	printf("\n\nWeightMatrix:\n");
	uint32_t dimension = getDimension();
	for (uint32_t i = 0u; i < dimension; ++i)
	{
		for (uint32_t j = 0u; j < dimension; ++j)
		{
			printf("%5d ", getWeight(i, j));
			if (j == (dimension - 1u))
			{
				printf("\n");
			}
		}
	}
	printf("\n\n");
}

void getMinWeightPath(uint32_t start, uint32_t dimension)
{
	/// Dijkstra O(n^2)
	assert(start < dimension && gVisited && gWeights);

	for (uint32_t i = 0u; i < dimension; ++i)
	{
		gWeights[i] = getWeight(start, i);
	}
	gVisited[start] = true;

	uint32_t counter = 1u;
	uint32_t next = 0u;
	while (counter < dimension - 1u)
	{
		int32_t minWeight = INT32_MAX;
		for (uint32_t i = 0u; i < dimension; ++i)
		{
			if (!gVisited[i] && gWeights[i] != INVALID_VALUE && gWeights[i] < minWeight)
			{
				minWeight = gWeights[i];
				next = i;
			}
		}

		gVisited[next] = true;

		for (uint32_t i = 0u; i < dimension; ++i)
		{
			if (!gVisited[i])
			{
				int32_t nextWeight = getWeight(next, i);
				if (nextWeight != -1 && (minWeight + nextWeight < gWeights[i]))
				{
					gWeights[i] = minWeight + nextWeight;
				}
			}
		}

		++counter;
	}
}

void calculateMinWeightPath(uint32_t start)
{
	/// So we send out (n-1) messengers from the capital to the rest of cities one time, 
	/// and let them make their every efforts to send the message to the target city as soon as possible,
	/// if anyone of them can't arrive his target cify, this mission is fail, and if they all succeed in the job, 
	/// the longest time of them will be the answer.
	uint32_t dimension = getDimension();
	getMinWeightPath(start, dimension);

	int32_t maxWeight = INVALID_VALUE;
	for (uint32_t i = 0u; i < dimension; ++i)
	{
		if (INVALID_VALUE == gWeights[i])
		{
			printf_s("There is no road from city %u to city %u, the message can't broadcast to the entire empire.\n", start + 1u, i + 1u);
			return;
		}

		maxWeight = max(gWeights[i], maxWeight);
	}

	assert(maxWeight != INVALID_VALUE);
	printf_s("The message broadcast to the entire empire takes %d time.\n", maxWeight);
}

#if 0
int32_t main()
{
	initialize(getDimensionFromInput());

	buildWeightMatrixFromInput();

#if defined(_DEBUG)
	outputWeightMatrix();
#endif

	calculateMinWeightPath(0u);

	finalize();

	return 0;
}
#endif