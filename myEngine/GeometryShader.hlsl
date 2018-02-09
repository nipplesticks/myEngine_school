cbuffer CONSTANT_BUFFER : register(b0)
{
    matrix WVPMatrix;
    matrix WorldMatrix;
};

cbuffer CAMERA_BUFFER : register(b1)
{
	float3 pos; 
	float3 lookAt; 
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
	float4 p0 = input[0].Pos;
	float4 p1 = input[1].Pos;
	float4 p2 = input[2].Pos;

    float3 v0 = mul(p1 - p0, WorldMatrix).xyz;
    float3 v1 = mul(p2 - p0, WorldMatrix).xyz;
    float3 n = normalize(cross(v0, v1));
	float3 normLookAt = normalize(lookAt); 
	n = normalize(mul(float4(n, 1), WorldMatrix).xyz); 
	float h = 0.0001f; 
	float angle = dot(n, normLookAt); 

	if (angle > h)
	{
		// Apply rotation	
		for (uint i = 0; i < 3; i++)
		{
			output.Pos = mul(input[i].Pos, WVPMatrix);
			//output.worldPos = mul(input[i].Pos, WorldMatrix);
			output.Normal = n;
			output.Tex = input[i].Tex;
			fragmentShaderInput.Append(output);
		}
	}	
    //fragmentShaderInput.RestartStrip();
}