#define Emitter  (0)
#define Particle (1)

cbuffer constantsBuffer
{
	float3 EmitPos;
	float ElapseTime;

	float3 EmitDir;
	float TotalTime;

	float4 EyePos;

	matrix VP;
}

const float3 g_Acceleration = { 0.0f, 7.8f, 0.0f };
const float2 g_QuadUV[4] = 
{
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f)
};

Texture2DArray TexArray;
Texture1D TexRandom;  /// Random texture used to generate random numbers in shaders.
SamplerState SamplerLinear;

struct ParticleVertex
{
	float3 InitialPos : Position;
	float3 InitialVelocity : Velocity;

	float2 Size : Size;
	float ElapseTime : ElapseTime;
	uint Type : Type;
};

float3 GetRandomFromTexture(float offset)
{
	/// Use game time plus offset to sample random texture.
	float U = TotalTime + offset;

	/// coordinates in [-1,1]
	float3 rand = TexRandom.SampleLevel(SamplerLinear, U, 0.0f).xyz;

	return normalize(rand);
}

ParticleVertex VSStreamOut(ParticleVertex input)
{
	return input;
}

/// The stream-out GS is just responsible for emitting new particles and destroying old particles. The logic programed 
/// here will generally vary from particle system to particle system, as the destroy/spawn rules will be different.
[maxvertexcount(2)]
void GSStreamOut(
	point ParticleVertex input[1], 
	inout PointStream<ParticleVertex> pointStream)
{
	input[0].ElapseTime += ElapseTime;

	if (input[0].Type == Emitter)
	{
		if (input[0].ElapseTime > 0.005f) /// Time to emit a new particle?
		{
			float3 rand = GetRandomFromTexture(0.0f);
			rand.x *= 0.5f;
			rand.z *= 0.5f;

			ParticleVertex v;
			v.InitialPos = EmitPos;
			v.InitialVelocity = rand * 4.0f;
			v.Size = float2(3.0f, 3.0f);
			v.ElapseTime = 0.0f;
			v.Type = Particle;

			pointStream.Append(v);

			/// Reset the time to emit
			input[0].ElapseTime = 0.0f;
        }

		/// Always keep emitters
		pointStream.Append(input[0]);
	}
	else if (input[0].Type == Particle)
	{
		/// Specify conditions to keep particle; this may vary from system to system.
		if (input[0].ElapseTime <= 1.0f)
		{
			pointStream.Append(input[0]);
		}
	}
}

struct VSOutput
{
	float3 Pos : Position;
	float4 Clr : Color;	
	float2 Size : Size;
	uint Type : Type;
};

VSOutput VSDraw(ParticleVertex input)
{
	VSOutput output;	

	/// Constant acceleration equation
	output.Pos = 0.5f * input.ElapseTime * input.ElapseTime * g_Acceleration + input.ElapseTime * input.InitialVelocity + input.InitialPos;

	/// Fade color with time
	float alpha = 1.0f - smoothstep(0.0f, 1.0f, input.ElapseTime / 1.0f);
	output.Clr = float4(1.0f, 1.0f, 1.0f, alpha);

	output.Size = input.Size;
	output.Type = input.Type;

	return output;
}

struct GSOutput
{
	float4 Pos : SV_Position;
	float4 Clr : COLOR;
	float2 UV : TEXCOORD;
};

/// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void GSDraw(point VSOutput input[1], inout TriangleStream<GSOutput> triangleStream)
{
	/// do not draw emitter particles.
	if (input[0].Type != Emitter)
	{
		// Compute world matrix so that billboard faces the camera.
		float3 lookAt = normalize(EyePos.xyz - input[0].Pos);
		float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), lookAt));
		float3 up = cross(lookAt, right);

		/// Compute triangle strip vertices (quad) in world space
		float halfWidth = 0.5f * input[0].Size.x;
		float halfHeight = 0.5f * input[0].Size.y;

		float4 v[4];
		v[0] = float4(input[0].Pos + halfWidth * right - halfHeight * up, 1.0f);
		v[1] = float4(input[0].Pos + halfWidth * right + halfHeight * up, 1.0f);
		v[2] = float4(input[0].Pos - halfWidth * right - halfHeight * up, 1.0f);
		v[3] = float4(input[0].Pos - halfWidth * right + halfHeight * up, 1.0f);

		// Transform quad vertices to world space and output them as a triangle strip.
		GSOutput output;

		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			output.Pos = mul(v[i], VP);
			output.UV = g_QuadUV[i];
			output.Clr = input[0].Clr;

			triangleStream.Append(output);
		}
    }
}

float4 PSMain(GSOutput input) : SV_Target
{
	return TexArray.Sample(SamplerLinear, float3(input.UV, 0.0f)) * input.Clr;
}