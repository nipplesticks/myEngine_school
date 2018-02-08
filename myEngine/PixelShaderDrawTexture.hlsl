Texture2D Texture;
SamplerState SampleType;

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

struct PS_OUT
{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
};

PS_OUT main(PS_IN input) : SV_Target
{
	PS_OUT output;
	output.diffuse = Texture.Sample(SampleType, input.Tex);
	output.normal = float4(input.Normal, 1);
	output.position = input.Pos;

	return output;

    /*float3 lightPos = float3(500, 5000, 500);
    
    float4 tx = Texture.Sample(SampleType, input.Tex);
    float4 amb = float4(0.2f, 0.2f, 0.2f, 1.0f);
    float3 dirToLight = normalize(lightPos - input.worldPos.xyz);
    float angle = dot(dirToLight, input.Normal);
    angle = max(angle, 0);


    float4 finalColor = amb * tx + (angle * tx);

    return float4(finalColor);*/
};