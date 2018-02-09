cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

struct VS_IN
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 Nor : NORMAL;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.Pos = mul(float4(input.Pos, 1), WVPMatrix);
    output.Tex = input.Tex;
    output.Nor = normalize(mul(float4(normalize(input.Nor), 1), WorldMatrix)).xyz;
    
    return output;
}