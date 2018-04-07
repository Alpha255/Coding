#include "Tessellator_Common.hlsli"

StructuredBuffer<float4> InputEdgeFactor : register(t0);
RWStructuredBuffer<uint2> NumVerticesIndices_Out : register(u0);

cbuffer cBuf : register(b1)
{
    uint4 VertexCount;
}

[numthreads(128, 1, 1)]
void Func_NumVI(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x < VertexCount.x)
    {
        float4 edgeFactor = InputEdgeFactor[dispatchThreadID.x];

        TessFactorTriangle processedTessFactor;
        int numPt = TriProcessTessFactor(edgeFactor, processedTessFactor);

        int numIndex = 0;
        if (0 == numPt)
        {
            numIndex = 0;
        }
        else if (3 == numPt)
        {
            numIndex = 4;
        }
		else
        {
			/// +1 is so even tess includes the center point, which we want to now
            int numRings = (processedTessFactor.OutInNumPoints.w + 1) / 2;

            int4 outInTessFactorHalf = int4(ceil(processedTessFactor.OutInTessFactorHalf));
            uint3 num = GetNumStitchTransition(outInTessFactorHalf, processedTessFactor.OutInTessFactorParity);

            numIndex = num.x + num.y + num.z;
            numIndex += GetTotalNumStitchRegular(true, Diagonals_Mirrored, processedTessFactor.OutInNumPoints.w, numRings - 1) * 3;

            if (TessFactor_Parity_Odd == processedTessFactor.OutInTessFactorParity.w)
            {
                numIndex += 4;
            }
        }

        NumVerticesIndices_Out[dispatchThreadID.x] = uint2(numPt, numIndex);
    }
}

