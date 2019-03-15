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

const float3 g_Acceleration = { -1.0f, -9.8f, 0.0f };

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
	return TexRandom.SampleLevel(SamplerLinear, U, 0.0f).xyz;
}

float3 GetUnitRandomFromTexture(float offset)
{
	return normalize(GetRandomFromTexture(offset));
}

ParticleVertex VSStreamOut(ParticleVertex input)
{
	return input;
}

/// The stream-out GS is just responsible for emitting new particles and destroying old particles. The logic programed 
/// here will generally vary from particle system to particle system, as the destroy/spawn rules will be different.
[maxvertexcount(6)]
void GSStreamOut(
	point ParticleVertex input[1], 
	inout PointStream<ParticleVertex> pointStream)
{
	input[0].ElapseTime += ElapseTime;

	if (input[0].Type == Emitter)
	{
		if (input[0].ElapseTime > 0.002f) /// Time to emit a new particle?
		{
			for (int i = 0; i < 5; ++i)
			{
				/// Spread rain drops out above the camera.
				float3 rand = 35.0f * GetRandomFromTexture((float)i / 5.0f);
			    rand.y = 20.0f;
			    
			    ParticleVertex v;
			    v.InitialPos = EmitPos + rand;
			    v.InitialVelocity = float3(0.0f, 0.0f, 0.0f);
			    v.Size = float2(1.0f, 1.0f);
			    v.ElapseTime = 0.0f;
			    v.Type = Particle;

				pointStream.Append(v);
			}

			/// Reset the time to emit
			input[0].ElapseTime = 0.0f;
        }

		/// Always keep emitters
		pointStream.Append(input[0]);
	}
	else if (input[0].Type == Particle)
	{
		/// Specify conditions to keep particle; this may vary from system to system.
		if (input[0].ElapseTime <= 3.0f)
		{
			pointStream.Append(input[0]);
		}
	}
}

struct VSOutput
{
	float3 Pos : Position;
	uint Type : Type;
};

VSOutput VSDraw(ParticleVertex input)
{
	VSOutput output;	

	/// Constant acceleration equation
	output.Pos = 0.5f * input.ElapseTime * input.ElapseTime * g_Acceleration + input.ElapseTime * input.InitialVelocity + input.InitialPos;
	output.Type = input.Type;

	return output;
}

struct GSOutput
{
	float4 Pos : SV_Position;
	float2 UV : TEXCOORD;
};

/// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void GSDraw(point VSOutput input[1], inout LineStream<GSOutput> lineStream)
{
	/// do not draw emitter particles.
	if (input[0].Type == Emitter)
	{
		return;
    }

	/// Slant line in acceleration direction.
	float3 p0 = input[0].Pos;
	float3 p1 = input[0].Pos + 0.07f * g_Acceleration;

	GSOutput v0;
	v0.Pos = mul(float4(p0, 1.0f), VP);
	v0.UV = float2(0.0f, 0.0f);
	lineStream.Append(v0);

	GSOutput v1;
	v1.Pos = mul(float4(p1, 1.0f), VP);
	v1.UV = float2(1.0f, 1.0f);
	lineStream.Append(v1);
}

float4 PSMain(GSOutput input) : SV_Target
{
	return TexArray.Sample(SamplerLinear, float3(input.UV, 0.0f));
}