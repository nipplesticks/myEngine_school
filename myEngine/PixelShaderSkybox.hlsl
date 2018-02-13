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

float4 main(PS_IN input) : SV_Target
{
    float4 tx = diffuse.Sample(SampleType, input.Tex);

    return float4(tx);
};