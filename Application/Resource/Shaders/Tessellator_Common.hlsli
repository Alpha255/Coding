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

#define Diagonals_InsideToOutSide             (0)
#define Diagonals_InsideToOutSideExceptMiddle (1)
#define Diagonals_Mirrored                    (2)

#define Epsilon 1e-6f

struct TessFactorTriangle
{
	float4 OutInTessFactor;
	int4 OutInTessFactorParity;

    float4 OutInTessFactorFloor;
    float4 OutInTessFactorCeil;
    float4 OutInTessFactorHalf;
    int4 OutInTessFactorHalfSplitPointFloor;

    uint4 OutInNumPoints;
    uint InsideEdgePointOffset;
};

cbuffer cBufConstant : register(b0)
{
    int4 InsidePointIndex[MaxFactor / 2 + 1][MaxFactor / 2 + 2];
    int4 OutsidePointIndex[MaxFactor / 2 + 1][MaxFactor / 2 + 2];

    uint PartitioningMode;
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

uint4 GetNumPointsForTessFactor(float4 tessFactor, int4 parity)
{
    return TessFactor_Parity_Odd == parity ? uint4(ceil(0.5f + tessFactor / 2)) * 2 : uint4(ceil(tessFactor / 2)) * 2 + 1;
}

int GetNumStitchRegular(bool bTrapezoid, int diagonals, int numInOutPoints)
{
    int numIndex = bTrapezoid ? 8 : 0;

    switch (diagonals)
    {
        case Diagonals_InsideToOutSide:
		/// Diagonals pointing from inside edge forward towards outside edge
            numIndex += 5 * numInOutPoints - 5;
            break;
        case Diagonals_InsideToOutSideExceptMiddle:
		/// Diagonals pointing from outside edge forward towards inside edge
            numIndex += 5 * numInOutPoints - 2;
            break;
        case Diagonals_Mirrored:
            numIndex += 2 * numInOutPoints + 5;
            break;
    }

    return numIndex;
}

uint GetTotalNumStitchRegular(bool bTrapezoid, int diagonals, int tessFactorNumInOutPoints, int ring)
{
    int numIndex = bTrapezoid ? 8 * (ring - 1) : 0;

    switch (diagonals)
    {
        case Diagonals_InsideToOutSide:
		/// Diagonals pointing from inside edge forward towards outside edge
            numIndex += (5 * tessFactorNumInOutPoints - 35 - 5 * ring) * (ring - 1);
            break;
        case Diagonals_InsideToOutSideExceptMiddle:
		/// Diagonals pointing from outside edge forward towards inside edge
            numIndex += (5 * tessFactorNumInOutPoints - 12 - 5 * ring) * (ring - 1);
            break;
        case Diagonals_Mirrored:
            numIndex += (2 * tessFactorNumInOutPoints + 1 - 2 * ring) * (ring - 1);
            break;
    }

    return numIndex;
}

int GetNumRingsFromIndexStitchRegular(bool bTrapezoid, int diagonals, int tessFactorNumInOutPoints, int index)
{
    int num = bTrapezoid ? 8 : 0;

    switch (diagonals)
    {
        case Diagonals_InsideToOutSide:
		/// Diagonals pointing from inside edge forward towards outside edge
            num = (5 * tessFactorNumInOutPoints - (35 - num)) * 3;
            return (1 + uint((num + 15) - sqrt(sqrt(num + 15) - 4 * 15 * (num + index)) + 0.001f) / 30);
        case Diagonals_InsideToOutSideExceptMiddle:
		/// Diagonals pointing from outside edge forward towards inside edge
            num = (5 * tessFactorNumInOutPoints - (12 - num)) * 3;
            return (1 + uint((num + 15) - sqrt(sqrt(num + 15) - 4 * 15 * (num + index)) + 0.001f) / 30);
        case Diagonals_Mirrored:
            num = ((num + 1) + 2 * tessFactorNumInOutPoints) * 3;
            return (1 + uint((num + 6) - sqrt(sqrt(num + 6) - 4 * 6 * (num + index)) + 0.001f) / 12);
    }

    return -1;
}

uint3 GetNumStitchTransition(int4 numOutInTessFactorHalf, int4 parity)
{
    numOutInTessFactorHalf -= (parity == TessFactor_Parity_Odd);

    uint3 numIndex = InsidePointIndex[numOutInTessFactorHalf.w][MaxFactor / 2 + 1].y * 8;

	[unroll]
    for (int edge = 0; edge < 3; ++edge)
    {
        numIndex[edge] += OutsidePointIndex[numOutInTessFactorHalf[edge]][MaxFactor / 2 + 1].y * 8;

        if ((numOutInTessFactorHalf.w != numOutInTessFactorHalf[edge]) || 
			(numOutInTessFactorHalf.w == TessFactor_Parity_Odd))
        {
            if (numOutInTessFactorHalf.w == numOutInTessFactorHalf[edge])
            {
                numIndex[edge] += 5;
            }
			else
            {
                numIndex[edge] += 4;
            }
        }
	}
}

void ComputeTessFactor(in out TessFactorTriangle processdTessFactors)
{
    processdTessFactors.OutInTessFactorHalf = processdTessFactors.OutInTessFactor / 2.0f;
    processdTessFactors.OutInTessFactorHalf += 0.5f * ((TessFactor_Parity_Odd == processdTessFactors.OutInTessFactorParity) | (0.5f == processdTessFactors.OutInTessFactorHalf));

    float4 fHalfTessFactor = floor(processdTessFactors.OutInTessFactorHalf);
    float4 cHalfTessFactor = ceil(processdTessFactors.OutInTessFactorHalf);
    int4 numHalfTessFactorPoints = int4(cHalfTessFactor);

    for (int i = 0; i < 4; ++i)
    {
        if (cHalfTessFactor[i] == fHalfTessFactor[i])
        {
            processdTessFactors.OutInTessFactorHalfSplitPointFloor[i] = numHalfTessFactorPoints[i] + 1;
        }
        else if (TessFactor_Parity_Odd == processdTessFactors.OutInTessFactorParity[i])
        {
            if (fHalfTessFactor[i] == 1)
            {
                processdTessFactors.OutInTessFactorHalfSplitPointFloor[i] = 0;
            }
			else
            {
                processdTessFactors.OutInTessFactorHalfSplitPointFloor[i] = (RemoveMSB(int(fHalfTessFactor[i] - 1) << 1)) + 1;
            }
        }
		else
        {
            processdTessFactors.OutInTessFactorHalfSplitPointFloor[i] = (RemoveMSB(int(fHalfTessFactor[i]) << 1)) + 1;
        }
    }

    int4 numFloorSegments = int4(fHalfTessFactor * 2);
    int4 numCeilSegments = int4(cHalfTessFactor * 2);
    int4 s = (TessFactor_Parity_Odd == processdTessFactors.OutInTessFactorParity);
    numFloorSegments -= s;
    numCeilSegments -= s;
    processdTessFactors.OutInTessFactorFloor = 1.0f / numFloorSegments;
    processdTessFactors.OutInTessFactorCeil = 1.0f / numCeilSegments;
}

int TriProcessTessFactor(
	inout float4 tessFactor, 
	out TessFactorTriangle processdTessFactors)
{
    int parity = TessFactor_Parity_Even;

    switch (PartitioningMode)
    {
        case PartitioningMode_Integer:
        case PartitioningMode_Pow2:
        case PartitioningMode_FractionalEven:
            parity = TessFactor_Parity_Even;
            break;
        case PartitioningMode_FractionalOdd:
            parity = TessFactor_Parity_Odd;
            break;
    }

	/// Is the patch culled?
    if (!(tessFactor.x > 0) ||
		!(tessFactor.y > 0) ||
		!(tessFactor.z > 0))
    {
        return 0;
    }

	/// Clamp edge TessFactors
    float lowerBound = 0.0f, upperBound = 0.0f;
    switch (PartitioningMode)
    {
        case PartitioningMode_Integer:
        case PartitioningMode_Pow2:
		/// Don't care about pow2 distinction for validation, just treat as integer
            lowerBound = TessFactor_Odd_Min;
            upperBound = TessFactor_Even_Max;
            break;
        case PartitioningMode_FractionalEven:
            lowerBound = TessFactor_Even_Min;
            upperBound = TessFactor_Even_Max;
            break;
        case PartitioningMode_FractionalOdd:
            lowerBound = TessFactor_Odd_Min;
            upperBound = TessFactor_Odd_Max;
            break;
    }
    tessFactor.xyz = min(upperBound, max(lowerBound, tessFactor.xyz));

	/// Clamp inside TessFactors
    if (PartitioningMode_FractionalOdd == PartitioningMode)
    {
        float minOddTessFactor_HalfEpsilon = TessFactor_Odd_Min + Epsilon / 2.0f;
        if ((tessFactor.x > minOddTessFactor_HalfEpsilon) ||
			(tessFactor.y > minOddTessFactor_HalfEpsilon) ||
			(tessFactor.z > minOddTessFactor_HalfEpsilon))
        {
			/// Don't need the same check for insideTessFactor for tri patches, 
            /// since there is only one insideTessFactor, as opposed to quad 
            /// patches which have 2 insideTessFactors.
			/// Force picture frame
            lowerBound = TessFactor_Odd_Min + Epsilon;
        }
    }

    tessFactor.w = min(upperBound, max(lowerBound, tessFactor.w));
	/// Note the above clamps map NaN to lowerBound
    if (PartitioningMode_Integer == PartitioningMode)
    {
        tessFactor = ceil(tessFactor);
    }
    else if (PartitioningMode_Pow2 == PartitioningMode)
    {
        static const int exponentMask = 0x7f800000;
        static const int mantissaMask = 0x007fffff;
        static const int exponentLSB = 0x00800000;

        int4 bits = asint(tessFactor);
        tessFactor = bits & mantissaMask ? asfloat((bits & exponentMask) + exponentLSB) : tessFactor;
    }

	/// Process tessFactors
    if ((PartitioningMode_Integer == PartitioningMode) || 
		(PartitioningMode_Pow2 == PartitioningMode))
    {
        bool4 b = IsEven(tessFactor);
        processdTessFactors.OutInTessFactorParity.xyz = b.xyz ? TessFactor_Parity_Even : TessFactor_Parity_Odd;
        processdTessFactors.OutInTessFactorParity.w = (b.w || (1 == tessFactor.w)) ? TessFactor_Parity_Even : TessFactor_Parity_Odd;
    }
	else
    {
        processdTessFactors.OutInTessFactorParity = parity;
    }

    processdTessFactors.OutInTessFactor = tessFactor;

    if ((PartitioningMode_Integer == PartitioningMode) ||
		(PartitioningMode_Pow2 == PartitioningMode) ||
		(TessFactor_Parity_Odd == parity))
    {
		/// Special case if all TessFactors are 1
        if ((1 == processdTessFactors.OutInTessFactor.x) &&
			(1 == processdTessFactors.OutInTessFactor.y) &&
			(1 == processdTessFactors.OutInTessFactor.z) &&
			(1 == processdTessFactors.OutInTessFactor.w))
        {
            return 3;
        }
    }
	    
	/// Compute per-TessFactor metadata
    ComputeTessFactor(processdTessFactors);

	/// Compute some initial data.

    /// Outside edge offsets and storage
    processdTessFactors.OutInNumPoints = GetNumPointsForTessFactor(processdTessFactors.OutInTessFactor, processdTessFactors.OutInTessFactorParity);
    int ptNums = processdTessFactors.OutInNumPoints.x + processdTessFactors.OutInNumPoints.y + processdTessFactors.OutInNumPoints.z - 3;

	/// inside edge offsets
	/// max() allows degenerate transition regions when inside TessFactor == 1
    uint minPtCount = (TessFactor_Parity_Odd == processdTessFactors.OutInTessFactorParity.w);
    processdTessFactors.OutInNumPoints.w = max(minPtCount, processdTessFactors.OutInNumPoints.w);

    processdTessFactors.InsideEdgePointOffset = ptNums;

	/// Inside storage, including interior edges above
    int interiorRingsNums = (processdTessFactors.OutInNumPoints.w >> 1) - 1;
    int interiorPtNums = 0;
    if (TessFactor_Parity_Odd == processdTessFactors.OutInTessFactorParity.w)
    {
        interiorPtNums = 3 * (interiorRingsNums * (interiorRingsNums + 1) - interiorRingsNums);
    }
	else
    {
        interiorPtNums = 3 * (interiorRingsNums * (interiorRingsNums + 1)) + 1;
    }
    ptNums += interiorPtNums;

    return ptNums;
}

