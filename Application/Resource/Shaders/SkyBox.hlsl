cbuffer cbVS
{
    matrix WVP;
};

TextureCube CubeMapTex;

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
	/// Set z = w so that z/w = 1 (i.e., skybox always on far plane).
	/// This means that the skybox will always fail the depth test against the other models in the scene. 
	/// That way the skybox will only take up the background left between the models and everything else will be infront of it,
	/// http://ogldev.atspace.co.uk/www/tutorial25/tutorial25.html
    output.PosH = mul(float4(input.Pos, 1.0f), WVP).xyww;

	/// Use local vertex position as cubemap lookup vector.
    output.PosL = input.Pos;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    return CubeMapTex.Sample(Sampler, input.PosL);
}