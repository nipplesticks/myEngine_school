Texture2D Texture : register(t1);
SamplerState SampleType;

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

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_OUT
{
    float4 diffuse : SV_Target0;
    float4 normal : SV_Target1;
    float4 wPosition : SV_Target2;
    float4 ambient : SV_target3;
    float4 specularHighlight : SV_target4;
    float4 LightPos : SV_target5;
    float4 LightDir : SV_target6;
    float4 Viewer : SV_target7;
};

PS_OUT main(PS_IN input) : SV_Target
{
	PS_OUT output;
	output.diffuse = Texture.Sample(SampleType, input.Tex);
	output.normal = float4(input.Normal, 1);
	output.wPosition = input.worldPos;
    output.specularHighlight = float4(0.2, 0.2, 0.2, 1);
    output.LightPos = float4(lightPosition, 1);
    output.LightDir = float4(normalize(lightPosition - input.worldPos.xyz), 0);
    output.Viewer = float4(normalize(camPos - input.worldPos.xyz), 0);

	return output;
};