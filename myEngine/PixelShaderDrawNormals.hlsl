//Texture2D txDiffuse : register(t0);
//SamplerState sampAni;

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 worldPos : WORLDPOS;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

float4 main(PS_IN input) : SV_Target
{
	float3 normal = normalize(input.Normal);

	return float4(normal, 1.0f);
};