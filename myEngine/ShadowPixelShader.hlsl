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
	float4 pos : SV_POSITION;
};

struct OUTPUT
{
	float4 pos : SV_TARGET0;
};

OUTPUT main(INPUT input)
{
	OUTPUT o;
	o.pos = input.pos;
	return o;
}