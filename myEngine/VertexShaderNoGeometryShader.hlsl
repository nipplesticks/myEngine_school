cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

cbuffer CAMERA_BUFFER : register(b1)
{
	float4 camPos;
	float4 lookAt;
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

float3x3 inverse3x3(float3x3 m)
{
	float invDet = 1 / determinant(m);

	float3x3 minv;


	minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
	minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
	minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
	minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
	minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
	minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invDet;
	minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
	minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invDet;
	minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;

	return minv;
}

float4x4 inverse(float4x4 input)
{
#define minor(a,b,c) determinant(float3x3(input.a, input.b, input.c))
	//determinant(float3x3(input._22_23_23, input._32_33_34, input._42_43_44))

	float4x4 cofactors = float4x4(
		minor(_22_23_24, _32_33_34, _42_43_44),
		-minor(_21_23_24, _31_33_34, _41_43_44),
		minor(_21_22_24, _31_32_34, _41_42_44),
		-minor(_21_22_23, _31_32_33, _41_42_43),

		-minor(_12_13_14, _32_33_34, _42_43_44),
		minor(_11_13_14, _31_33_34, _41_43_44),
		-minor(_11_12_14, _31_32_34, _41_42_44),
		minor(_11_12_13, _31_32_33, _41_42_43),

		minor(_12_13_14, _22_23_24, _42_43_44),
		-minor(_11_13_14, _21_23_24, _41_43_44),
		minor(_11_12_14, _21_22_24, _41_42_44),
		-minor(_11_12_13, _21_22_23, _41_42_43),

		-minor(_12_13_14, _22_23_24, _32_33_34),
		minor(_11_13_14, _21_23_24, _31_33_34),
		-minor(_11_12_14, _21_22_24, _31_32_34),
		minor(_11_12_13, _21_22_23, _31_32_33)
		);
#undef minor
	return transpose(cofactors) / determinant(input);
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
	float3x3 TBN : TEXCOORD2;
};



VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.Pos = mul(float4(input.Pos, 1), WVPMatrix);
	output.wPos = mul(float4(input.Pos, 1), WorldMatrix);
    output.Tex = input.Tex;
    output.Nor = normalize(mul(float4(input.Nor, 0), WorldMatrix)).xyz;

	/*float4x4 wM = WorldMatrix;

	float3 t = mul(normalize(input.tan), wM);
	float3 b = mul(normalize(input.bi), wM);
	float3 n = mul(normalize(input.Nor), wM);

	float3x3 worldToTangent = float3x3(t, b, n);
	output.TBN = transpose(worldToTangent);*/



    output.TBN[0] = normalize(mul(input.tan, WorldMatrix));
	output.TBN[1] = normalize(mul(input.bi, WorldMatrix));
	output.TBN[2] = normalize(mul(input.Nor, WorldMatrix));

	//https://digitalerr0r.wordpress.com/2012/03/03/xna-4-0-shader-programming-4normal-mapping/
    return output;
}