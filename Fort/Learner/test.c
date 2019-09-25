#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

enum
{
	eMaxDimension = 100
};

int32_t *gWeightMatrix = NULL;

void swap(uint32_t *pLeft, uint32_t *pRight)
{
	uint32_t temp = *pLeft;
	*pLeft = *pRight;
	*pRight = temp;
}

uint32_t getDimension()
{
	assert(gWeightMatrix);

	uint32_t dimension = gWeightMatrix[0];
	assert(dimension <= eMaxDimension);
	return dimension;
}

int32_t getWeight(uint32_t row, uint32_t col)
{
	assert(gWeightMatrix && row < getDimension() && col < getDimension());

	if (row == col)
	{
		return 0;
	}

	if (row < col)
	{
		swap(&row, &col);
	}

	return gWeightMatrix[((row * (row - 1u)) >> 1u) + col + 1u];
}

void setWeight(uint32_t row, uint32_t col, int32_t weight)
{
	assert(gWeightMatrix && row < getDimension() && col < getDimension());

	gWeightMatrix[((row * (row - 1u)) >> 1u) + col + 1u] = weight;
}

void initialize(uint32_t dimension)
{
	assert(dimension >= 1u && dimension <= eMaxDimension);
	uint32_t length = ((dimension * (dimension - 1u)) >> 1u) + 1u;
	gWeightMatrix = (int32_t *)malloc(sizeof(int32_t) * length);
	assert(gWeightMatrix);
	memset(gWeightMatrix, -1, sizeof(int32_t) * length);

	gWeightMatrix[0] = dimension;
}

void finalize()
{
	if (gWeightMatrix)
	{
		free(gWeightMatrix);
		gWeightMatrix = NULL;
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
	while (dimension > eMaxDimension)
	{
		printf_s("Please input the count of cities (less than %u): ", eMaxDimension);
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
			else if (dimension > eMaxDimension)
			{
				printf_s("The count of cities must less than %u, please try again.\n\n", eMaxDimension);
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
					setWeight(row, col++, -1);
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
						printf_s("Invalid input format, please try again.\n\n");
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

void getMinWeightPath(uint32_t start)
{
	assert(start < getDimension());
}

int32_t main()
{
	initialize(getDimensionFromInput());

	buildWeightMatrixFromInput();

#if defined(_DEBUG)
	outputWeightMatrix();
#endif

	getMinWeightPath(0u);

	finalize();

	return 0;
}