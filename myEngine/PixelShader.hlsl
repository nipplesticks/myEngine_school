Texture2D txDiffuse : register(t0);
SamplerState sampAni;

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 Tex : TEXCOORD;
};

float4 main(PS_IN input) : SV_Target
{
    //float4 tx = txDiffuse.Sample(sampAni, input.Tex);
    
    return float4(0.0, 0.0, 0.0, 0.0);
};