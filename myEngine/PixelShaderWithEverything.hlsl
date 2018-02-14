Texture2D ambientTx : register(t0);
Texture2D diffuseTx : register(t1);
Texture2D specularHighlightTx : register(t2);
Texture2D normalMap : register(t3);

SamplerState SampleType;

cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WVPMatrix;
	matrix WorldMatrix;
};

cbuffer CAMERA_BUFFER : register(b1)
{
	float3 camPos;
	float3 lookAt;
	matrix viewMatrix;
}

cbuffer LIGHT_BUFFER : register(b2)
{
	float3 lightPosition;
	float3 lightColor;
	float strength;
}

cbuffer CONSTANT_BUFFER2 : register(b3)
{
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
	float2 Tex : TEXCOORD0;
	float3x3 TBN : TEXCOORD2;
};

struct PS_OUT
{
	float4 diffuse					: SV_Target0;
	float4 normal					: SV_Target1;
	float4 wPosition				: SV_Target2;
	float4 ambient					: SV_target3;
	float4 specularHighlight		: SV_target4;
};

PS_OUT main(PS_IN input) : SV_Target
{
	PS_OUT output = (PS_OUT)0;
	
	float3 normal = (2.0f * normalMap.Sample(SampleType, input.Tex) - 1.0).xyz;
	normal = normalize(mul(normal, input.TBN));
	output.normal = float4(normalize(input.Nor + normal), 0.0f);

	//output.normal = float4(input.Nor, 0.0f);

	output.wPosition = input.wPos;

	output.diffuse = diffuseTx.Sample(SampleType, input.Tex);
	output.specularHighlight = specularHighlightTx.Sample(SampleType, input.Tex);
	output.ambient = ambientTx.Sample(SampleType, input.Tex);

	return output;
};