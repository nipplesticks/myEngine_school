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
	float3 pos : POSITION;
};
struct OUTPUT
{
	float4 pos : SV_POSITION;
};

OUTPUT main( INPUT input)
{
	OUTPUT output;
	float4 position = float4(input.pos, 1);
	position = mul(position, WorldMatrix);
	position = mul(position, lightView);
	position = mul(position, lightProj);

	output.pos = position;
	return output;
}