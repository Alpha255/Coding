StructuredBuffer<uint2> Input : register(t0);
StructuredBuffer<uint2> Input1 : register( t1 );
RWStructuredBuffer<uint2> Output : register(u0);

groupshared uint4 bucket[128];

void Scan(uint3 dispatchedThreadID, uint groupIndex, uint2 x)
{
	/// since CS40 can only support one shared memory for one shader, we use .xy and .zw as ping-ponging buffers
    /// if scan a single element type like int, search and replace all .xy to .x and .zw to .y below
    bucket[groupIndex].xy = x;
    bucket[groupIndex].zw = 0;

	/// Up sweep
	[unroll]
    for (uint stride = 2; stride <= 128; stride <<= 1)
    {
		GroupMemoryBarrierWithGroupSync();

		if ((groupIndex & (stride - 1)) == (stride - 1))
        {
			bucket[groupIndex].xy += bucket[groupIndex - stride / 2].xy;
        }
    }

	if (groupIndex == (128 - 1))
    {
		bucket[groupIndex].xy = 0;
    }

	/// Down sweep
	bool b = true;

	[unroll]
	for (stride = 128 / 2; stride >= 1; stride >>= 1)
    {
		GroupMemoryBarrierWithGroupSync();

		uint a = stride - 1;
		uint b = stride | a;

		/// ping-pong between passes
		if (b)
        {
			if ((groupIndex & b) == b)
            {
				bucket[groupIndex].zw = bucket[groupIndex - stride].xy + bucket[groupIndex].xy;
            }
            else if ((groupIndex & a) == a)
            {
				bucket[groupIndex].zw = bucket[groupIndex + stride].xy;
            }
			else
            {
				bucket[groupIndex].zw = bucket[groupIndex].xy;
            }
        }
		else
        {
			if ((groupIndex & b) == b)
            {
				bucket[groupIndex].xy = bucket[groupIndex - stride].zw + bucket[groupIndex].zw;
            }
			else if ((groupIndex & a) == a)
            {
				bucket[groupIndex].xy = bucket[groupIndex + stride].zw;
            }
			else
            {
				bucket[groupIndex].xy = bucket[groupIndex].zw;
            }
        }

		b = !b;
    }

	Output[dispatchedThreadID.x] = bucket[groupIndex].zw + x;
}

/// Scan in each bucket
[numthreads(128, 1, 1)]
void Func_ScanInBucket(uint3 dispatchedThreadID : SV_DispatchThreadID, uint3 groudThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	uint2 x = Input[dispatchedThreadID.x];
	Scan(dispatchedThreadID, groupIndex, x);
}

/// Record and scan the sum of each bucket
[numthreads(128, 1, 1)]
void Func_ScanBucketResult(uint3 dispatchedThreadID : SV_DispatchThreadID, uint3 groudThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	uint2 x = Input[dispatchedThreadID.x * 128 - 1];
	Scan(dispatchedThreadID, groupIndex, x);
}

[numthreads(128, 1, 1)]
void Func_ScanAddBucketResult(uint3 groupID : SV_GroupID, uint3 dispatchedThreadID : SV_DispatchThreadID, uint3 groudThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex)
{
	Output[dispatchedThreadID.x] = Input[dispatchedThreadID.x] + Input1[groupID.x];
}