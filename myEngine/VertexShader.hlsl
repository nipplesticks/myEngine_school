cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

struct VS_IN
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.Pos = float4(input.Pos, 1);
    output.Tex = input.Tex;
    return output;
}