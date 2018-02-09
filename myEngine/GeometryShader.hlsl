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

struct GS_IN
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float3 Nor : NORMAL;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : WORLDPOS;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};


[maxvertexcount(3)]
void main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> fragmentShaderInput)
{
	GS_OUT output = (GS_OUT)0;
    // set normal
	float4 p0 = mul(input[0].Pos, WorldMatrix);
	float4 p1 = mul(input[1].Pos, WorldMatrix);
	float4 p2 = mul(input[2].Pos, WorldMatrix);

    float3 v0 = (p1 - p0).xyz;
    float3 v1 = (p2 - p0).xyz;
	float3 n = normalize(cross(v0, v1));

	float4 vP0 = mul(p0, viewMatrix);
	float4 vP1 = mul(p1, viewMatrix);
	float4 vP2 = mul(p2, viewMatrix);

	float3 vV0 = (vP1 - vP0).xyz;
	float3 vV1 = (vP2 - vP0).xyz;

	float3 vN = normalize(cross(vV1, vV0));

	if (dot(-vP0, vN) < 0)
	{
		// Apply rotation	
		for (uint i = 0; i < 3; i++)
		{
			output.Pos = mul(input[i].Pos, WVPMatrix);
			output.worldPos = mul(input[i].Pos, WorldMatrix);
			output.Normal = n;
			output.Tex = input[i].Tex;
			fragmentShaderInput.Append(output);
		}
	}	
    //fragmentShaderInput.RestartStrip();
}