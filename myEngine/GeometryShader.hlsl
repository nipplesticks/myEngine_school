struct GS_IN
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct GS_OUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};


[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> fragmentShaderInput)
{
	GS_OUT output = (GS_OUT)0;

	for (uint i = 0; i < 3; i++)
	{
		output.Pos = input[i].Pos;
		output.Tex = input[i].Tex;
		fragmentShaderInput.Append(output);
	}
	fragmentShaderInput.RestartStrip();
}