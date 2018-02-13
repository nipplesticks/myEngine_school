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

cbuffer LIGHT_BUFFER : register(b2)
{
    float3 lightPosition;
    float3 lightColor;
    float strength;
}

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
    float4 wPos : WORLDPOS;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 View : TEXCOORD1;
    float3x3 WorldToTangentSpace : TEXCOORD2;
};

float4 main(PS_IN input) : SV_Target
{
    return float4(lightColor, 1);
};