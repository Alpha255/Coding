struct SurfaceData
{
	float LinearDepth;
	float3 Color;
	float3 Normal;
	float SpecPow;
	float SpecIntensity;
};

Texture2D DepthTexture;
Texture2D ColorSpecIntTexture;
Texture2D NormalTexture;
Texture2D SpecPowTexture;

float ConvertToLinearDepth(float depth, float4 pespective)
{
	return pespective.z / (depth + pespective.w);	
}