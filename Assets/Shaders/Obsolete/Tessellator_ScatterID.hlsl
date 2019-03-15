#include "Tessellator_Common.hlsli"

StructuredBuffer<uint2> InputScanned : register(t0);
RWStructuredBuffer<uint2> TriIDIndexID_Out : register(u0);

cbuffer cBuf : register(b1)
{
    uint4 VertexCount;
}

[numthreads(128, 1, 1)]
void Func_ScatterVertexTriID_IndexID(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x < VertexCount.x)
    {
        uint start = InputScanned[dispatchThreadID.x - 1].x;
        uint end = InputScanned[dispatchThreadID.x].x;

        for (uint i = start; i < end; ++i)
        {
            TriIDIndexID_Out[i] = uint2(dispatchThreadID.x, i - start);
        }
    }
}

[numthreads(128, 1, 1)]
void Func_ScatterIndexTriID_IndexID(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x < VertexCount.x)
    {
        uint start = InputScanned[dispatchThreadID.x - 1].y;
        uint end = InputScanned[dispatchThreadID.x].y;

        for (uint i = start; i < end; ++i)
        {
            TriIDIndexID_Out[i] = uint2(dispatchThreadID.x, i - start);
        }
    }
}

