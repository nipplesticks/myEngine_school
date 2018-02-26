Texture2D Texture : register(t1);
SamplerState SampleType;

cbuffer CAMERA_BUFFER : register(b1)
{
    float3 camPos;
    float3 lookAt;
    matrix viewMatrix;
}

cbuffer LIGHT_BUFFER : register(b5)
{
	float4 lightPosition;
	float4 lightColor;
	float4 lightDir;
	matrix lightProj;
	matrix lightView;
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
};

PS_OUT main(PS_IN input) : SV_Target
{
	PS_OUT output;
	output.diffuse = Texture.Sample(SampleType, input.Tex);
	output.normal = float4(input.Normal, 1);
	output.wPosition = input.worldPos;
    output.specularHighlight = float4(float3(0.5, 0.5, 0.5) * lightColor.rgb, 1);

	return output;
};