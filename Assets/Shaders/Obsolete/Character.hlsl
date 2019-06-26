Texture2D DiffuseTex : register(t0);
SamplerState LinearSampler : register(s0);

cbuffer cbVS
{
    matrix WVP;
};

cbuffer cbPS
{
	float4 ColorMaskWhite;
	float4 ColorMaskBlack;
	float4 ColorMaskGrey80;
	float4 ColorMaskGrey150;
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
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

float4 photoshop_blend_overlay ( float4 base, float4 blend )
{
	return (base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend) ) );
}

float3 photoshop_blend_overlay ( float3 base, float3 blend )
{
	return (base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend) ) );
}

VSOutput VSMain(VSInput vsInput)
{
    VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    float4 texClr = DiffuseTex.Sample(LinearSampler, psInput.UV);

	float limitA = 0;
	float limitB = 0.25;
    float limitC = 0.5;
    float limitD = 0.75;
    float factor = 2;

    float _192255_10 = step(limitD, texClr.b);
    float4 Operator48_11 = ColorMaskWhite * _192255_10;
    float Operator64_13 = _192255_10 * factor;
    float Operator63_14 = texClr.b - Operator64_13;
    float _128192_16 = step(limitC, Operator63_14);
    
    float4 Operator54_17 = ColorMaskGrey150 * _128192_16;
    float4 Operator57_18 = Operator48_11 + Operator54_17;
    float Operator641_19 = _128192_16 * factor;
	float Operator631_20 = Operator63_14 - Operator641_19;

	float _64128_22 = step(limitB, Operator631_20);
	float4 Operator55_23 = ColorMaskGrey80 * _64128_22;
	float Operator642_24 = _64128_22 * factor;
	float Operator632_25 = Operator631_20 - Operator642_24;

	float _064_27 = step(limitA, Operator632_25);
	float4 Operator56_28 = ColorMaskBlack * _064_27;
	float4 Operator58_29 = Operator55_23 + Operator56_28;
	float4 Operator59_30 = Operator57_18 + Operator58_29;
	float4 DiffuseColorMask_31 = Operator59_30 * texClr.r;

	float4 Operator272_34 = float4(0.0, 0.0, 0.0, 1.0);
	float4 Operator268_35 = photoshop_blend_overlay( float4(DiffuseColorMask_31.xyz, 1.0 ) , float4(Operator272_34.xyz, 1.0 ) ) ;
	float4 Operator271_36 = lerp(float4( DiffuseColorMask_31.xyz, 1.0 ), float4( Operator268_35.xyz, 1.0 ) , _064_27 );

	float4 Operator273_39 = float4(0.0, 0.0, 0.0, 1.0);
	float4 Operator270_40 = photoshop_blend_overlay( float4(DiffuseColorMask_31.xyz, 1.0 ) , float4(Operator273_39.xyz, 1.0 ) ) ;
	float4 Operator269_41 = lerp(float4( DiffuseColorMask_31.xyz, 1.0 ), float4( Operator270_40.xyz, 1.0 ) , _192255_10 );
	
	float4 Albedo = Operator271_36 + Operator269_41 - DiffuseColorMask_31;

	return Albedo;
}