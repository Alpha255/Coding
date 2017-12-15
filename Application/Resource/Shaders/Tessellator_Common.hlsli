#define MaxFactor 16

#define TessFactor_Parity_Even  (0)
#define TessFactor_Parity_Odd   (1)

#define PartitioningMode_Integer        (0)
#define PartitioningMode_Pow2           (1)
#define PartitioningMode_FractionalOdd  (2)
#define PartitioningMode_FractionalEven (3)

#define TessFactor_Odd_Min  (1)
#define TessFactor_Odd_Max  (63)
#define TessFactor_Even_Min (2)
#define TessFactor_Even_Max (64)

#define Epsilon 1e-6f

struct TessFactorTriangle
{
	float4 OutInTessFactor;
	int4 OutInTessFactorParity;
};

cbuffer cBufConstant : register(b0)
{
    int4 InsidePointIndex[MaxFactor / 2 + 1][MaxFactor / 2 + 2];
    int4 OutsidePointIndex[MaxFactor / 2 + 1][MaxFactor / 2 + 2];

    uint PartitionMode;
    uint3 Placeholder;
}

bool4 IsEven(float4 input)
{
    return (((uint4) input) & 1) ? false : true;
}

uint RemoveMSB(uint val)
{
    int check;
    if (val <= 0x0000ffff)
    {
        check = (val <= 0x000000ff) ? 0x00000080 : 0x00008000;
    }
    else
    {
        check = (val <= 0x00ffffff) ? 0x00800000 : 0x80000000;
    }
    for (int i = 0; i < 8; i++, check >>= 1)
    {
        if (val & check)
        {
            return (val & ~check);
        }
    }
    return 0;
}

uint4 NumPointsForTessFactor(float4 tessFactor, int4 parity)
{
    return TessFactor_Parity_Odd == parity ? uint4(ceil(0.5f + tessFactor / 2)) * 2 : uint4(ceil(tessFactor / 2)) * 2 + 1;
}

void ComputeTessFactor(
	float4 tessFactor, 
	int4 parity,
	out float4 invNumSegsFloorTessFactor,
	out float4 invNumSegsCeilTessFactor,
	out float4 halfTessFactor, 
	out int4 splitPtFloorHalfTessFactor)
{
    halfTessFactor = tessFactor / 2.0f;
    halfTessFactor += 0.5f * ((TessFactor_Parity_Odd == parity) | (0.5f == halfTessFactor));

    float4 fHalfTessFactor = floor(halfTessFactor);
    float4 cHalfTessFactor = ceil(halfTessFactor);
    int4 numHalfTessFactorPoints = int4(cHalfTessFactor);

    for (int i = 0; i < 4; ++i)
    {
        if (cHalfTessFactor[i] == fHalfTessFactor[i])
        {
            splitPtFloorHalfTessFactor[i] = numHalfTessFactorPoints[i] + 1;
        }
        else if (TessFactor_Parity_Odd == parity[i])
        {
            if (fHalfTessFactor[i] == 1)
            {
                splitPtFloorHalfTessFactor[i] = 0;
            }
			else
            {
                splitPtFloorHalfTessFactor[i] = (RemoveMSB(int(fHalfTessFactor[i] - 1) << 1)) + 1;
            }
        }
		else
        {
            splitPtFloorHalfTessFactor[i] = (RemoveMSB(int(fHalfTessFactor[i]) << 1)) + 1;
        }
    }

    int4 numFloorSegments = int4(fHalfTessFactor * 2);
    int4 numCeilSegments = int4(cHalfTessFactor * 2);
    int4 s = (TessFactor_Parity_Odd == parity);
    numFloorSegments -= s;
    numCeilSegments -= s;
    invNumSegsFloorTessFactor = 1.0f / numFloorSegments;
    invNumSegsCeilTessFactor = 1.0f / numCeilSegments;
}

int TriProcessTessFactor(
	inout float4 tessFactor, 
	out TessFactorTriangle tessFactors,
	int paititioningMode)
{
    int parity = TessFactor_Parity_Even;

    switch (paititioningMode)
    {
    	case 
    }
}