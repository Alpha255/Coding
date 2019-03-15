#include "Tessellator_Common.hlsli"

cbuffer cBuf : register(b1)
{
    uint4 VertexCount;
}

StructuredBuffer<uint2> InputTriagleIDIndexID : register(t0);
StructuredBuffer<float4> InputEdgeFactor : register(t1);

struct TessedVertex
{
    uint TriangleID;
    float2 UV;
};
RWStructuredBuffer<TessedVertex> TessedVertices_Out : register(u0);

void PlacePointIn1D(TessFactorTriangle processedTessFactor, int index, int pt, out float location, int parity)
{
    int numHalfTessFactorPoints = int(ceil(processedTessFactor.OutInNumPoints[index]));

    bool bFlip = false;
    if (pt >= numHalfTessFactorPoints)
    {
        pt = (numHalfTessFactorPoints << 1) - pt;
        if (TessFactor_Parity_Odd == parity)
        {
            pt -= 1;
        }

        bFlip = true;
    }
	else
    {
        bFlip = false;
    }

	if (pt == numHalfTessFactorPoints)
    {
        location = 0.5f;
    }
	else
    {
        uint indexHalfTessFactorCeil = pt;
        uint indexHalfTessFactorFloor = pt;
		if (pt > processedTessFactor.OutInTessFactorHalfSplitPointFloor[index])
        {
            indexHalfTessFactorFloor -= 1;
        }

        float locHalfTessFactorFloor = indexHalfTessFactorFloor * processedTessFactor.OutInTessFactorFloor[index];
        float locHalfTessFactorCeil = indexHalfTessFactorCeil * processedTessFactor.OutInTessFactorCeil[index];

        location = lerp(locHalfTessFactorFloor, locHalfTessFactorFloor, frac(processedTessFactor.OutInTessFactorHalf[index]));

		if (bFlip)
        {
            location = 1.0f - location;
        }
    }
}

[numthreads(128, 1, 1)]
void Func_TessVertex(uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint id = dispatchThreadID.x;

	if (id < VertexCount.x)
    {
        uint triangleID = InputTriagleIDIndexID[id].x;
        uint vertexID = InputTriagleIDIndexID[id].y;

        float4 outInFactor = InputEdgeFactor[triangleID];

        TessFactorTriangle processedTessFactor;
        int numPoints = TriProcessTessFactor(outInFactor, processedTessFactor);

        float2 UV = float2(0.0f, 0.0f);
		if (3 == numPoints)
        {
            UV = ((1 == vertexID) ? float2(0.0f, 0.0f) : ((0 == vertexID) ? float2(0.0f, 1.0f) : float2(1.0f, 0.0f)));
        }
		else
        {
			if (vertexID < processedTessFactor.InsideEdgePointOffset)
            {
				/// Generate exterior ring edge points, clockwise starting from point V (VW, the U==0 edge)
                int edge = 0;
				if (vertexID < processedTessFactor.OutInNumPoints.x - 1)
                {
                    edge = 0;
                }
				else
                {
                    vertexID -= processedTessFactor.OutInNumPoints.x - 1;
					if (vertexID < processedTessFactor.OutInNumPoints.y - 1)
                    {
                        edge = 1;
                    }
					else
                    {
                        vertexID -= processedTessFactor.OutInNumPoints.y - 1;
                        edge = 2;
                    }
                }

                int p = vertexID;
                int endPoint = processedTessFactor.OutInNumPoints[edge] - 1;
                float param = 0.0f;
				/// Whether to reverse point order given we are defining V or U (W implicit):
				/// edge0, VW, has V decreasing, so reverse 1D points below
				/// edge1, WU, has U increasing, so don't reverse 1D points  below
				/// edge2, UV, has U decreasing, so reverse 1D points below
                int q = (edge & 0x1) ? p : endPoint - p;
                PlacePointIn1D(processedTessFactor, edge, q, param, processedTessFactor.OutInTessFactorParity[edge]);

                UV = ((1 == edge) ? float2(param, 0.0f) : ((0 == edge) ? float2(0.0f, param) : float2(param, 1.0f - param)));
            }
			else
            {
				/// Generate interior ring points, clockwise spiralling in
                uint index = vertexID - processedTessFactor.InsideEdgePointOffset;
                uint ring = 1 + (((3 * processedTessFactor.OutInNumPoints.w - 6) - sqrt(sqrt(3 * processedTessFactor.OutInNumPoints.w - 6) - 4 * 3 * index)) + 0.001f) / 6;
                index -= 3 * (processedTessFactor.OutInNumPoints.w - ring - 1) * (ring - 1);

                uint startPoint = ring;
                uint endPoint = processedTessFactor.OutInNumPoints.w - 1 - startPoint;

                if (index < 3 * (endPoint - startPoint))
                {
                    uint edge = index / (endPoint - startPoint);
                    uint p = index - edge * (endPoint - startPoint) + startPoint;

                    int perpendicularAxisPoint = startPoint;
                    float perpParam = 0.0f;
                    PlacePointIn1D(processedTessFactor, 3, perpendicularAxisPoint, perpParam, processedTessFactor.OutInTessFactorParity.w);
                    perpParam = perpParam * 2.0f / 3.0f;

                    float param = 0.0f;
                    int q = (edge & 0x1) ? p : endPoint - (p - startPoint);
					/// Whether to reverse point given we are defining V or U (W implicit):
					/// edge0, VW, has V decreasing, so reverse 1D points below
					/// edge1, WU, has U increasing, so don't reverse 1D points  below
					/// edge2, UV, has U decreasing, so reverse 1D points below
                    PlacePointIn1D(processedTessFactor, 3, q, param, processedTessFactor.OutInTessFactorParity.w);

					/// edge0 VW, has perpendicular parameter U constant
                    /// edge1 WU, has perpendicular parameter V constant
                    /// edge2 UV, has perpendicular parameter W constant 
                    /// reciprocal is the rate of change of edge-parallel parameters as they are pushed into the triangle
					const uint deriv = 2;
                    UV = ((1 == edge) ? float2(param - perpParam / deriv, perpParam) : ((0 == edge) ? float2(perpParam, param - perpParam / deriv) : float2(param - perpParam / deriv, 1 - (param - perpParam / deriv + perpParam))));
                }
				else
                {
                    if (processedTessFactor.OutInTessFactorParity.w != TessFactor_Parity_Odd)
                    {
						/// Last point is the point at the center.
                        UV = 1 / 3.0f;
                    }
                }
            }
        }

        TessedVertices_Out[id].TriangleID = triangleID;
        TessedVertices_Out[id].UV = UV;
    }
}