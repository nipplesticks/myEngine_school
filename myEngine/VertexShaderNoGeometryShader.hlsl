cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

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

struct VS_IN
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 Nor : NORMAL;
	float3 bi  : BITANGENT;
	float3 tan : TANGENT;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
	float4 wPos : WORLDPOS;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD0;
	float3 View : TEXCOORD1;
	float3x3 WorldToTangentSpace : TEXCOORD2;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.Pos = mul(float4(input.Pos, 1), WVPMatrix);
	output.wPos = mul(float4(input.Pos, 1), WorldMatrix);
    output.Tex = input.Tex;
    output.Nor = normalize(mul(float4(input.Nor, 0), WorldMatrix)).xyz;

    output.WorldToTangentSpace[0] = mul(normalize(input.tan), WorldMatrix);
	output.WorldToTangentSpace[1] = mul(normalize(input.bi), WorldMatrix);
	output.WorldToTangentSpace[2] = mul(normalize(input.Nor), WorldMatrix);
	output.View = normalize(float4(camPos, 1.0f) - output.wPos);



	//https://digitalerr0r.wordpress.com/2012/03/03/xna-4-0-shader-programming-4normal-mapping/
    return output;
}