Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specularHighlight : register(t2);
Texture2D normalMap : register(t3);

SamplerState SampleType;

cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

cbuffer CONSTANT_BUFFER : register(b3)
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
    float2 Tex : TEXCOORD;
};

struct PS_OUT
{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
	float4 wPosition : SV_Target2;
	float4 ambient : SV_target3;
	float4 specularHighlight : SV_target4;
};

PS_OUT main(PS_IN input) : SV_Target
{
	PS_OUT output = (PS_OUT)0;
    output.diffuse = diffuse.Sample(SampleType, input.Tex);
	output.normal = float4(0.0, 0.0, 0.0, 0.0);



    return output;
};