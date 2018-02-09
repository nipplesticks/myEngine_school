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

VS_OUT main(VS_IN i, uint id : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	if (id == 0)
	{
		output.Pos = float4(-1.0f, -1.0f, 0.0f, 1.0f);
		output.Tex = float2(0.0f, 1.0f);
	}
	else if (id == 1)
	{
		output.Pos = float4(-1.0f, 1.0f, 0.0f, 1.0f);
		output.Tex = float2(0.0f, 0.0f);
	}
	else if (id == 2)
	{
		output.Pos = float4(1.0f, -1.0f, 0.0f, 1.0f);
		output.Tex = float2(1.0f, 1.0f);
	}
	else if (id == 3)
	{
		output.Pos = float4(1.0f, 1.0f, 0.0f, 1.0f);
		output.Tex = float2(1.0f, 0.0f);
	}
	return output;
}