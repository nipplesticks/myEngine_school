Texture2D ambientTx : register(t0);
Texture2D diffuseTx : register(t1);
Texture2D specularHighlightTx : register(t2);
Texture2D normalMap : register(t3);

SamplerState SampleType;

cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WVPMatrix;
	matrix WorldMatrix;
	float3 ambientLevel;
	float3 diffuseLevel;
	float4 specularLevel;
	float3 emissiveLevel;
	float dissolveLevel;
	float opticalDensityLevel;
	float sharpnessLevel;
};


struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 wPos : WORLDPOS;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD;
	float3 TLightPos : TANGENTLIGHTPOS;
	float3 TViewPos : TANGENTVIEWPOS;
	float3 TPos : TANGENTPOS;
};

struct PS_OUT
{
	float4 diffuse				: SV_Target0;
	float4 normal				: SV_Target1;
	float4 wPosition			: SV_Target2;
	float4 ambient				: SV_target3;
	float4 specularHighlight	: SV_target4;
	float4 TLightPos			: SV_Target5;
	float4 TViewPos				: SV_Target6;
	float4 TPos					: SV_Target7;
};

PS_OUT main(PS_IN input) : SV_Target
{
	PS_OUT output = (PS_OUT)0;
	output.diffuse = diffuseTx.Sample(SampleType, input.Tex);
	output.normal = normalMap.Sample(SampleType, input.Tex);
	output.wPosition = input.wPos;
	output.ambient = ambientTx.Sample(SampleType, input.Tex);
	output.specularHighlight = specularHighlightTx.Sample(SampleType, input.Tex);

	output.TLightPos = float4(input.TLightPos, 1);
	output.TViewPos = float4(input.TViewPos, 1);
	output.TPos = float4(input.TPos, 1);


	return output;
};