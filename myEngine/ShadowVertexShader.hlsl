cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WVPMatrix;
	matrix WorldMatrix;
};
cbuffer LIGHT_BUFFER : register(b2)
{
	float3 lightPosition;
	float3 lightColor;
	float strength;
	matrix lightDir;
	matrix lightProj;
	matrix lightView;
}

struct INPUT
{
	float3 pos : POSITION;
};
struct OUTPUT
{
	float4 pos : SV_POSITION;
};


OUTPUT main( INPUT input)
{
	OUTPUT output;
	matrix wvp = lightProj * lightView * WorldMatrix;
	output.pos = mul(float4(input.pos, 1), wvp);

	return output;
}