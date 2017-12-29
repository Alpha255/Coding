cbuffer cBuf
{
    matrix WVP;

    float2 TessEdgeLenScale;
    uint TriCount;
    uint PartitioningMode;
};

/////////////////////////////////// Axis-Tests //////////////////////////////////////////
bool AxisTest(uint3 axis, float3 v0, float3 v1, float3 boxHalfSize, float2 ab, float2 fab)
{
    float factor = 1.0f;
    float2 box = float2(0.0f, 0.0f);

	float p0 = 0.0f, p1 = 0.0f;

    if (axis.x == 1)
    {
        box = boxHalfSize.yz;

		p0 = factor * ab.x * v0.y - factor * ab.y * v0.z;
        p1 = factor * ab.x * v1.y - factor * ab.y * v1.z;
    }
	else if (axis.y == 1)
    {
        factor = -1.0f;
        box = boxHalfSize.xz;

		p0 = factor * ab.x * v0.x - factor * ab.y * v0.z;
        p1 = factor * ab.x * v1.x - factor * ab.y * v1.z;
    }
	else if (axis.z == 1)
    {
        box = boxHalfSize.xy;

		p0 = factor * ab.x * v0.x - factor * ab.y * v0.y;
        p1 = factor * ab.x * v1.x - factor * ab.y * v1.y;
    }

    float minV = min(p0, p1);
    float maxV = max(p0, p1);

    float rad = dot(fab, box);

    return (minV < rad) && (maxV > -rad);
}
///////////////////////////////////////////////////////////////////////////////////////

bool IsPlaneBoxOverlap(float3 normal, float d, float3 maxBox)
{
    float3 vmin = maxBox, vmax = maxBox;

	[unroll]
    for (int i = 0; i <= 2; ++i)
    {
        if (normal[i] > 0.0f)
        {
            vmin[i] *= -1;
        }
        else
        {
            vmax[i] *= -1;
        }
    }

    if (dot(normal, vmin) + d > 0.0f)
    {
        return false;
    }

    if (dot(normal, vmax) + d >= 0.0f)
    {
        return true;
    }

    return false;
}


bool IsTriangleBoxOverlap(float3 boxCenter, float3 boxHalfSize, float3 triP0, float3 triP1, float3 triP2)
{
	/// https://github.com/JChehe/blog/issues/8
	/// http://blog.csdn.net/yorhomwang/article/details/54869018
	/// https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
	/*  use separating axis theorem to test overlap between triangle and box need to test for overlap in these directions: */
    /*  1) the {x,y,z}-directions (actually, since we use the AABB of the triangle we do not even need to test these) */
    /*  2) normal of the triangle */
    /*  3) crossproduct(edge from tri, {x,y,z}-directin) this gives 3x3=9 more tests */

    float3 v0 = triP0 - boxCenter;
    float3 v1 = triP1 - boxCenter;
    float3 v2 = triP2 - boxCenter;

    float3 e0 = v1 - v0;
    float3 e1 = v2 - v1;
    float3 e2 = v0 - v2;

    float3 fe = abs(e0);
    if (!AxisTest(uint3(1, 0, 0), v0, v2, boxHalfSize, e0.zy, fe.zy) || 
		!AxisTest(uint3(0, 1, 0), v0, v2, boxHalfSize, e0.zx, fe.zx) || 
		!AxisTest(uint3(0, 0, 1), v1, v2, boxHalfSize, e0.yx, fe.yx))
    {
        return false;
    }

    fe = abs(e1);
    if (!AxisTest(uint3(1, 0, 0), v0, v2, boxHalfSize, e0.zy, fe.zy) ||
		!AxisTest(uint3(0, 1, 0), v0, v2, boxHalfSize, e0.zx, fe.zx) ||
		!AxisTest(uint3(0, 0, 1), v0, v1, boxHalfSize, e0.yx, fe.yx))
    {
        return false;
    }

    fe = abs(e2);
    if (!AxisTest(uint3(1, 0, 0), v0, v1, boxHalfSize, e0.zy, fe.zy) ||
		!AxisTest(uint3(0, 1, 0), v0, v1, boxHalfSize, e0.zx, fe.zx) ||
		!AxisTest(uint3(0, 0, 1), v1, v2, boxHalfSize, e0.yx, fe.yx))
    {
        return false;
    }

	/*  first test overlap in the {x,y,z}-directions */
    /*  find min, max of the triangle each direction, and test for overlap in */
    /*  that direction -- this is equivalent to testing a minimal AABB around */
    /*  the triangle against the AABB */
    float3 minV = min(min(v0, v1), v2);
    float3 maxV = max(max(v0, v1), v2);
    if ((minV.x > boxHalfSize.x || maxV.x < -boxHalfSize.x) ||
		(minV.y > boxHalfSize.y || maxV.y < -boxHalfSize.y) ||
		(minV.z > boxHalfSize.z || maxV.z < -boxHalfSize.z))
    {
        return false;
    }

    float3 normal = cross(e0, e1);
    float d = -dot(normal, v0);
    if (!IsPlaneBoxOverlap(normal, d, boxHalfSize))
    {
        return false;
    }

    return true;
}

Buffer<float4> InputVertices : register(t0);
RWStructuredBuffer<float4> EdgeFactorBuffer_Out : register(u0);

[numthreads(128, 1, 1)]
void Func_EdgeFactor(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x < TriCount)
    {
        float4 p0 = mul(InputVertices[dispatchThreadID.x * 3 + 0], WVP);
        float4 p1 = mul(InputVertices[dispatchThreadID.x * 3 + 1], WVP);
        float4 p2 = mul(InputVertices[dispatchThreadID.x * 3 + 2], WVP);

        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;

        float4 factor;
		/// Only triangles which are completely inside or intersect with the view frustum are taken into account 
        if (IsTriangleBoxOverlap(float3(0.0f, 0.0f, 0.5f), float3(1.02f, 1.02f, 0.52f), p0.xyz, p1.xyz, p2.xyz))
        {
            factor.x = length((p0.xy - p2.xy) * TessEdgeLenScale);
            factor.y = length((p1.xy - p0.xy) * TessEdgeLenScale);
            factor.z = length((p2.xy - p1.xy) * TessEdgeLenScale);
            factor.w = min(min(factor.x, factor.y), factor.z);
            factor = clamp(factor, 0.0f, 9.0f);
        }
        else
        {
            factor = 0.0f;
        }

        EdgeFactorBuffer_Out[dispatchThreadID.x] = factor;
    }
}