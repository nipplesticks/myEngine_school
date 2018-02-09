SamplerState sampAni;

Texture2D gTexDiffuse : register(t0);
Texture2D gTexNormal : register(t1);
Texture2D gTexPosition : register(t2);

//struct Light
//{
//	float3 dir;
//	float4 ambient;
//	float4 diffuse;
//};
//
//cbuffer PER_FRAME_BUFF : register(b1)
//{
//	Light light;
//	//float4x4 view;
//};

struct VS_OUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
	//float3 Normal : NORMAL;
	//float4x4 mulMatrix;
};

float4 main(VS_OUT input) : SV_Target
{
	float4 pOut;
float3 normal = gTexNormal.Sample(sampAni, input.Tex).rgb;
float3 diffuse = gTexDiffuse.Sample(sampAni, input.Tex).rgb;

if (length(normal) > 0.0f)
{
	float3 lightDir = normalize(float3(1,1,1));
	float3 position = gTexPosition.Sample(sampAni, input.Pos).rgb; //float3(1, 1, 1); //float3(view._41, view._42, view._43);

	float lambertian = max(dot(lightDir, normal), 0.0f);
	float specular = 0.0f;

	[flatten]
	if (lambertian > 0.0f)
	{
		float3 viewDir = normalize(-position);
		float3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(halfDir, normal), 0.0f);
		specular = pow(specAngle, 30.0f);
	}

	float3 colorLinear = lambertian * diffuse + specular * float3(1.0f, 1.0f, 1.0f);
	pOut = float4(pow(colorLinear, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f)), 1.0f);
	return pOut;
}

pOut = float4(diffuse, 1.0f);
return pOut;
}