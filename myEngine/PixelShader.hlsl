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
    //float4 tx = txDiffuse.Sample(sampAni, input.Tex);
    float3 lightPos = float3(3, 5, -2);
    float3 lightDir = normalize(lightPos - input.worldPos.xyz);
    float diffuse = dot(lightDir, input.Normal);
    float3 finalColor = 0.2 * float3(1.0f, 0.85892157f, 0.67459216f) + float3(1.0f, 0.980392f, 0.95686275f) * diffuse;

    return float4(finalColor, 1.0f);
};