Texture2D Texture;
SamplerState SampleType;

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

float4 main(PS_IN input) : SV_Target
{
    float4 tx = Texture.Sample(SampleType, input.Tex);
    return float4(tx);
};