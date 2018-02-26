cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WVPMatrix;
	matrix WorldMatrix;
};
cbuffer LIGHT_BUFFER : register(b5)
{
	float4 lightPosition;
	float4 lightColor;
	float4 lightDir;
	matrix lightProj;
	matrix lightView;
	float strength;
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