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
    float2 Tex : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.Pos = float4(input.Pos, 1);
    
    output.Pos = mul(output.Pos, WVPMatrix).xyzw;
    output.Pos.z = output.Pos.w - 0.0000001f;

    output.Tex = input.Tex;



    return output;
}