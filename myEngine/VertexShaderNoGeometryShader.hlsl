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
    float2 Tex : TEXCOORD;
	float3 TLightPos : TANGENTLIGHTPOS;
	float3 TViewPos : TANGENTVIEWPOS;
	float3 TPos : TANGENTPOS;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.Pos = mul(float4(input.Pos, 1), WVPMatrix);
	output.wPos = mul(float4(input.Pos, 1), WorldMatrix);
    output.Tex = input.Tex;
    output.Nor = normalize(mul(float4(input.Nor, 1), WorldMatrix)).xyz;

	

	float3 t = normalize(mul(float4(input.tan, 1), WorldMatrix)).xyz;
	float3 n = normalize(mul(float4(input.Nor, 1), WorldMatrix)).xyz;
	float3 b = normalize(mul(float4(input.bi, 1), WorldMatrix)).xyz;
    
	//t = normalilze(t - dot(t, n));

	float3x3 TBN = float3x3(t, b, n);
	output.TLightPos = mul(lightPosition, TBN);
	output.TViewPos = mul(camPos, TBN);
	output.TPos = mul(output.wPos.xyz, TBN);
    return output;
}