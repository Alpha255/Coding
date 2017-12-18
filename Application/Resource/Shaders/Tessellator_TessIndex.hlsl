#include "Tessellator_Common.hlsli"

StructuredBuffer<uint2> InputTriIDIndexID : register(t0);
StructuredBuffer<float4> InputEdgeFactor : register(t1);
StructuredBuffer<uint2> InputScanned : register(t2);

RWByteAddressBuffer TessedIndicesOut : register(u0);

cbuffer cbCS : register(b1)
{
    uint4 VertexCount;
}


struct IndexPatch
{
    int insidePointIndexDeltaToRealValue;
    int insidePointIndexBadValue;
    int insidePointIndexReplacementValue;
    int outsidePointIndexPatchBase;
    int outsidePointIndexDeltaToRealValue;
    int outsidePointIndexBadValue;
    int outsidePointIndexReplacementValue;
};

int TransformIndex1(int index, int vertices_base)
{
    return vertices_base + index;
}

int TransformIndex2(int index, int vertices_base, IndexPatch IndexPatchContext)
{
    if (index >= IndexPatchContext.outsidePointIndexPatchBase) /// Assumed remapped outide indices are > remapped inside vertices
    {
        if (index == IndexPatchContext.outsidePointIndexBadValue)
        {
            index = IndexPatchContext.outsidePointIndexReplacementValue;
        }
        else
        {
            index += IndexPatchContext.outsidePointIndexDeltaToRealValue;
        }
    }
    else
    {
        if (index == IndexPatchContext.insidePointIndexBadValue)
        {
            index = IndexPatchContext.insidePointIndexReplacementValue;
        }
        else
        {
            index += IndexPatchContext.insidePointIndexDeltaToRealValue;
        }
    }

    return vertices_base + index;
}


int AStitchRegular(
	bool bTrapezoid, 
	int diagonals,
	uint numInsideEdgePoints,
	int2 outsideInsideEdgePointBaseOffset,
	int i
	)
{
    if (bTrapezoid)
    {
        ++outsideInsideEdgePointBaseOffset.x;
    }

    int pt;

    if ((i < 4) && bTrapezoid)
    {
        if (i < 2)
        {
            pt = outsideInsideEdgePointBaseOffset.x - 1 + i;
        }
        else if (i == 2)
        {
            pt = outsideInsideEdgePointBaseOffset.y;
        }
        else
        {
            pt = -1;
        }
    }

    int index = i;
    if (bTrapezoid)
    {
        index -= 4;
    }

    if (index >= 0)
    {
        uint uindex = (uint) index;
        
        switch (diagonals)
        {
            case Diagonals_InsideToOutSide:
                if (uindex < 5 * numInsideEdgePoints - 5)
                {
                    uint p = uindex / 5;
                    uint r = uindex - p * 5;
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + p + r;
                    }
                    else if (r < 4)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + p + r;
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                else
                {
                    int r = i - (4 + 5 * numInsideEdgePoints - 5);
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + numInsideEdgePoints - 1 + r;
                    }
                    else if (r == 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + numInsideEdgePoints - 1;
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                break;

            case Diagonals_InsideToOutSideExceptMiddle: /// Assumes ODD tessellation
                if (uindex < (numInsideEdgePoints / 2 - 1) * 5)
                {
                /// First half
                    uint p = uindex / 5;
                    uint r = uindex - p * 5;
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + p + r;
                    }
                    else if (r < 4)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + p;
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                else if (uindex < (numInsideEdgePoints / 2 - 1) * 5 + 8)
                {
                /// Middle
                    uint r = uindex - (numInsideEdgePoints / 2 - 1) * 5;
                    if (0 == r)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + numInsideEdgePoints / 2 - 1;
                    }
                    else if (r < 3)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + numInsideEdgePoints / 2 - 1 + (2 - r);
                    }
                    else if (r == 3)
                    {
                        pt = -1;
                    }
                    else if (r < 6)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + numInsideEdgePoints / 2 - 1 + (r - 4);
                    }
                    else if (r == 6)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + numInsideEdgePoints / 2 - 1 + 1;
                    }
                    else if (r == 7)
                    {
                        pt = -1;
                    }
                }
				///else if (uindex < (numInsideEdgePoints/2-1) * 5 + 8 + (numInsideEdgePoints - numInsideEdgePoints/2 - 1) * 5)
                else if (uindex < numInsideEdgePoints * 5 - 2)
                {
                /// Second half
                    uint p = (uindex - (numInsideEdgePoints / 2 - 1) * 5 + 8) / 5 + numInsideEdgePoints / 2 + 1;
                    uint r = uindex - (numInsideEdgePoints / 2 - 1) * 5 + 8 - (p - (numInsideEdgePoints / 2 + 1)) * 5;
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + p - 1 + r;
                    }
                    else if (r < 4)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + p - 1 + r;
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                else
                {
					/// int r = i - (4 + (numInsideEdgePoints/2-1) * 5 + 8 + (numInsideEdgePoints - numInsideEdgePoints/2 - 1) * 5);
                    int r = i - (numInsideEdgePoints * 5 + 2);
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + numInsideEdgePoints - 1 + r;
                    }
                    else if (r == 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + numInsideEdgePoints - 1;
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                break;

            case Diagonals_Mirrored:
                if (uindex < (numInsideEdgePoints / 2 + 1) * 2)
                {
                    uint p = uindex / 2;
                    uint r = uindex - p * 2;
                    if (0 == r)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + p;
                    }
                    else
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + p;
                    }
                }
                else if (uindex == (numInsideEdgePoints / 2 + 1) * 2)
                {
                    pt = -1;
                }
                else if (uindex == (numInsideEdgePoints / 2 + 1) * 2 + 1)
                {
                    pt = outsideInsideEdgePointBaseOffset.x + numInsideEdgePoints / 2;
                }
				/// else if (uindex < (numInsideEdgePoints / 2 + 1) * 2 + 2 + (numInsideEdgePoints - numInsideEdgePoints / 2) * 2)
                else if (uindex < numInsideEdgePoints * 2 + 4)
                {
                    uint p = (uindex - ((numInsideEdgePoints / 2 + 1) * 2 + 2)) / 2 + numInsideEdgePoints / 2;
                    uint r = uindex - ((numInsideEdgePoints / 2 + 1) * 2 + 2) - (p - numInsideEdgePoints / 2) * 2;
                    if (0 == r)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + p;
                    }
                    else
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + p;
                    }
                }
				/// else if (uindex == (numInsideEdgePoints / 2 + 1) * 2 + 2 + (numInsideEdgePoints - numInsideEdgePoints / 2) * 2)
                else if (uindex == numInsideEdgePoints * 2 + 4)
                {
                    pt = -1;
                }
                else
                {
					/// int r = i - (4 + (numInsideEdgePoints / 2 + 1) * 2 + 2 + (numInsideEdgePoints - numInsideEdgePoints / 2) * 2 + 1);
                    uint r = i - (numInsideEdgePoints * 2 + 9);
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + numInsideEdgePoints - 1 + r;
                    }
                    else if (r == 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + numInsideEdgePoints - 1;
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                break;
        }
    }

    return pt;
}

int AStitchTransition(
	int2 outsideInsideEdgePointBaseOffset, 
	int2 outsideInsideNumHalfTessFactorPoints,
	int2 outsideInsideEdgeTessFactorParity,
	uint i)
{
    outsideInsideNumHalfTessFactorPoints -= (TessFactor_Parity_Odd == outsideInsideEdgeTessFactorParity);
    
    uint2 out_in_first_half = uint2(OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][MaxFactor / 2 + 1].y, InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][MaxFactor / 2 + 1].y) * 4;

    uint3 out_in_middle = 0;
    if ((outsideInsideEdgeTessFactorParity.y != outsideInsideEdgeTessFactorParity.x) || (outsideInsideEdgeTessFactorParity.y == TessFactor_Parity_Odd))
    {
        if (outsideInsideEdgeTessFactorParity.y == outsideInsideEdgeTessFactorParity.x)
        {
            // Quad in the middle
            out_in_middle.z = 5;
            out_in_middle.xy = 1;
        }
        else if (TessFactor_Parity_Even == outsideInsideEdgeTessFactorParity.y)
        {
            // Triangle pointing inside
            out_in_middle.z = 4;
            out_in_middle.x = 1;
        }
        else
        {
            // Triangle pointing outside
            out_in_middle.z = 4;
            out_in_middle.y = 1;
        }
    }


    int pt = -1;

    if (i < out_in_first_half.y)
    {
        // Advance inside

        uint p = i / 4;
        uint r = i - p * 4;
        p = InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p].z;
        if ((0 == r) || (2 == r))
        {
            pt = outsideInsideEdgePointBaseOffset.y + InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p].y + r / 2;
        }
        else if (1 == r)
        {
            pt = outsideInsideEdgePointBaseOffset.x + OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p].y;
        }
    }
    else
    {
        i -= out_in_first_half.y;
        
        if (i < out_in_first_half.x)
        {
            // Advance outside

            uint p = i / 4;
            uint r = i - p * 4;
            p = OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p].z;
            if (r < 2)
            {
                pt = outsideInsideEdgePointBaseOffset.x + OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p].y + r;
            }
            else if (r == 2)
            {
                pt = outsideInsideEdgePointBaseOffset.y + InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p].y;
                if (InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p].x)
                {
                    ++pt;
                }
            }
        }
        else
        {
            i -= out_in_first_half.x;
            
            if (i < out_in_middle.z)
            {
                uint r = i;
                if (outsideInsideEdgeTessFactorParity.y == outsideInsideEdgeTessFactorParity.x)
                {
                    // Quad in the middle
                    if ((0 == r) || (2 == r))
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + out_in_first_half.y / 4 + (2 == r); //r / 2;
                    }
                    else if ((1 == r) || (3 == r))
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + out_in_first_half.x / 4 + (3 == r); //(r - 1) / 2;
                    }
                }
                else if (TessFactor_Parity_Even == outsideInsideEdgeTessFactorParity.y)
                {
                    // Triangle pointing inside
                    if (r == 0)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + out_in_first_half.y / 4;
                    }
                    else if (r < 3)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + out_in_first_half.x / 4 + r - 1;
                    }
                }
                else
                {
                    // Triangle pointing outside
                    if ((0 == r) || (2 == r))
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + out_in_first_half.y / 4 + (2 == r); //r / 2;
                    }
                    else if (1 == r)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + out_in_first_half.x / 4;
                    }
                }
            }
            else
            {
                i -= out_in_middle.z;
                
                if (i < out_in_first_half.x)
                {
                    // Advance outside

                    uint p = i / 4;
                    uint r = i - p * 4;
                    p = OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p].z;
                    if (r < 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + out_in_first_half.x / 4 + out_in_middle.x + (OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][MaxFactor / 2 + 1].y - OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p + 1].y) + r;
                    }
                    else if (r == 2)
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + out_in_first_half.y / 4 + out_in_middle.y + (InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][MaxFactor / 2 + 1].y - InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p + 1].y);
                    }
                }
                else
                {
                    // Advance inside
                    
                    i -= out_in_first_half.x;

                    uint p = i / 4;
                    uint r = i - p * 4;
                    p = InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p].w;
                    if ((0 == r) || (2 == r))
                    {
                        pt = outsideInsideEdgePointBaseOffset.y + out_in_first_half.y / 4 + out_in_middle.y + (InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][MaxFactor / 2 + 1].y - InsidePointIndex[outsideInsideNumHalfTessFactorPoints.y][p + 1].y) + (2 == r); //r / 2;
                    }
                    else if (1 == r)
                    {
                        pt = outsideInsideEdgePointBaseOffset.x + out_in_first_half.x / 4 + out_in_middle.x + (OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][MaxFactor / 2 + 1].y - OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p + 1].y);
                        if (OutsidePointIndex[outsideInsideNumHalfTessFactorPoints.x][p].x)
                        {
                            ++pt;
                        }
                    }
                }
            }
        }
    }

    return pt;
}

[numthreads(128, 1, 1)]
void Func_TessIndex(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
    uint id = DTid.x;
    //uint id = Gid.x * 128 + GI; // Workaround for some CS4x preview drivers
    
    if (id < VertexCount.x)
    {
        uint tri_id = InputTriIDIndexID[id].x;
        uint index_id = InputTriIDIndexID[id].y;
        uint base_vertex = InputScanned[tri_id - 1].x;
        
        float4 outside_inside_factor = InputEdgeFactor[tri_id];
        
        TessFactorTriangle processedTessFactors;
        int num_points = TriProcessTessFactor(outside_inside_factor, processedTessFactors);

        uint tessed_indices;
        if (3 == num_points)
        {
            if (index_id < 3)
            {
                tessed_indices = TransformIndex1(index_id, base_vertex);
            }
            else
            {
                tessed_indices = -1;
            }
        }
        else
        {
            // Generate primitives for all the concentric rings, one side at a time for each ring
            static const int startRing = 1;
            int numRings = ((processedTessFactors.OutInNumPoints.w + 1) / 2); // +1 is so even tess includes the center point, which we want to now

            int4 outsideInsideHalfTessFactor = int4(ceil(processedTessFactors.OutInTessFactorHalf));
            uint3 num = GetNumStitchTransition(outsideInsideHalfTessFactor, processedTessFactors.OutInTessFactorParity);
            num.y += num.x;
            num.z += num.y;
            uint num_index = num.z;
            num_index += GetTotalNumStitchRegular(true, Diagonals_Mirrored, processedTessFactors.OutInNumPoints.w, numRings - 1) * 3;
            if (processedTessFactors.OutInTessFactorParity.w == TessFactor_Parity_Odd)
            {
                num_index += 4;
            }

            int pt;

            if (index_id < num.x)
            {
                int numPointsForInsideEdge = processedTessFactors.OutInNumPoints.w - 2 * startRing;

                pt = AStitchTransition(
					int2(0, processedTessFactors.InsideEdgePointOffset),
                    outsideInsideHalfTessFactor.xw,
                    processedTessFactors.OutInTessFactorParity.xw,
                    index_id);

                if (pt != -1)
                {
                    pt = TransformIndex1(pt, base_vertex);
                }
            }
            else if (index_id < num.y)
            {
                int numPointsForInsideEdge = processedTessFactors.OutInNumPoints.w - 2 * startRing;

                pt = AStitchTransition(
                        int2(processedTessFactors.OutInNumPoints.x - 1, processedTessFactors.InsideEdgePointOffset + numPointsForInsideEdge - 1),
                        outsideInsideHalfTessFactor.yw,
                        processedTessFactors.OutInTessFactorParity.yw,
                        index_id - num.x);
                if (pt != -1)
                {
                    pt = TransformIndex1(pt, base_vertex);
                }
            }
            else if (index_id < num.z)
            {
                int numPointsForInsideEdge = processedTessFactors.OutInNumPoints.w - 2 * startRing;

                IndexPatch IndexPatchContext;
                IndexPatchContext.insidePointIndexDeltaToRealValue = processedTessFactors.InsideEdgePointOffset + 2 * (numPointsForInsideEdge - 1);
                IndexPatchContext.insidePointIndexBadValue = numPointsForInsideEdge - 1;
                IndexPatchContext.insidePointIndexReplacementValue = processedTessFactors.InsideEdgePointOffset;
                IndexPatchContext.outsidePointIndexPatchBase = IndexPatchContext.insidePointIndexBadValue + 1; // past inside patched index range
                IndexPatchContext.outsidePointIndexDeltaToRealValue = processedTessFactors.OutInNumPoints.x + processedTessFactors.OutInNumPoints.y - 2 - IndexPatchContext.outsidePointIndexPatchBase;
                IndexPatchContext.outsidePointIndexBadValue = IndexPatchContext.outsidePointIndexPatchBase + processedTessFactors.OutInNumPoints.z - 1;
                IndexPatchContext.outsidePointIndexReplacementValue = 0;

                pt = AStitchTransition(
					int2(numPointsForInsideEdge, 0),
                    outsideInsideHalfTessFactor.zw,
                    processedTessFactors.OutInTessFactorParity.zw,
                    index_id - num.y);
                if (pt != -1)
                {
                    pt = TransformIndex2(pt, base_vertex, IndexPatchContext);
                }
            }
            else
            {
                if ((processedTessFactors.OutInTessFactorParity.w == TessFactor_Parity_Odd) && (index_id >= num_index - 4))
                {
                    int outsideEdgePointBaseOffset = processedTessFactors.OutInNumPoints + ((processedTessFactors.OutInNumPoints.w + 1) - (numRings + startRing)) * (numRings - startRing - 1) * 3;

                    if (index_id - (num_index - 4) != 3)
                    {
                        pt = TransformIndex1(outsideEdgePointBaseOffset + index_id - (num_index - 4), base_vertex);
                    }
                    else
                    {
                        pt = -1;
                    }
                }
                else
                {
                    int ring = GetNumRingsFromIndexStitchRegular(true, Diagonals_Mirrored, processedTessFactors.OutInNumPoints.w, index_id - num.z);

                    int tn = GetTotalNumStitchRegular(true, Diagonals_Mirrored, processedTessFactors.OutInNumPoints.w, ring - 1) * 3;
                    int n = GetNumStitchRegular(true, Diagonals_Mirrored, processedTessFactors.OutInNumPoints.w - 2 * ring);

                    int edge = (index_id - num.z - tn) / n;
                    int index = (index_id - num.z - tn) - edge * n;

                    int2 outsideInsideEdgePointBaseOffset = processedTessFactors.InsideEdgePointOffset + int2(0, 3 * (processedTessFactors.OutInNumPoints.w - 3)) + ((processedTessFactors.OutInNumPoints.w - (ring + startRing)) + int2(1, -1)) * (ring - startRing - 1) * 3;

                    int numPointsForInsideEdge = processedTessFactors.OutInNumPoints.w - 2 * ring;
                    int numLastPointsForInsideEdge = numPointsForInsideEdge + 2;

                    if (edge < 2)
                    {
                        pt = AStitchRegular(
							true, 
							Diagonals_Mirrored,
                            numPointsForInsideEdge,
                            outsideInsideEdgePointBaseOffset + (int2(numLastPointsForInsideEdge, numPointsForInsideEdge) - 1) * edge,
                            index);
                        if (pt != -1)
                        {
                            pt = TransformIndex1(pt, base_vertex);
                        }
                    }
                    else
                    {
                        IndexPatch IndexPatchContext;
                        IndexPatchContext.insidePointIndexDeltaToRealValue = outsideInsideEdgePointBaseOffset.y + (numPointsForInsideEdge - 1) * 2;
                        IndexPatchContext.insidePointIndexBadValue = numPointsForInsideEdge - 1;
                        IndexPatchContext.insidePointIndexReplacementValue = outsideInsideEdgePointBaseOffset.y;
                        IndexPatchContext.outsidePointIndexPatchBase = IndexPatchContext.insidePointIndexBadValue + 1; // past inside patched index range
                        IndexPatchContext.outsidePointIndexDeltaToRealValue = outsideInsideEdgePointBaseOffset.x + (numLastPointsForInsideEdge - 1) * 2 - IndexPatchContext.outsidePointIndexPatchBase;
                        IndexPatchContext.outsidePointIndexBadValue = IndexPatchContext.outsidePointIndexPatchBase + numLastPointsForInsideEdge - 1;
                        IndexPatchContext.outsidePointIndexReplacementValue = outsideInsideEdgePointBaseOffset.x;

                        pt = AStitchRegular(
							true, 
							Diagonals_Mirrored,
                            numPointsForInsideEdge,
                            int2(numPointsForInsideEdge, 0),
                            index);
                        if (pt != -1)
                        {
                            pt = TransformIndex2(pt, base_vertex, IndexPatchContext);
                        }
                    }
                }
            }

            tessed_indices = pt;
        }

        TessedIndicesOut.Store(id * 4, tessed_indices);
    }
}
