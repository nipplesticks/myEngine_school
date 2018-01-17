cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

struct GS_IN
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct GS_OUT
{
    float4 Pos      : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float3 Normal   : NORMAL;
    float2 Tex      : TEXCOORD;
};


[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> fragmentShaderInput)
{
	GS_OUT output = (GS_OUT)0;
    // set normal
    float4 p0 = input[0].Pos;
    float4 p1 = input[1].Pos;
    float4 p2 = input[2].Pos;

    float3 v0 = p1.xyz - p0.xyz;
    float3 v1 = p2.xyz - p0.xyz;
    float3 n = normalize(cross(v0, v1));

	// Apply rotation
    for (uint i = 0; i < 3; i++)
    {
        output.Pos = mul(input[i].Pos, WVPMatrix);
        output.worldPos = mul(input[i].Pos, WorldMatrix);
        output.Normal = mul(float4(n, 0), WorldMatrix).xyz;
        output.Tex = input[i].Tex;
        fragmentShaderInput.Append(output);
    }
		
    fragmentShaderInput.RestartStrip();
}