SamplerState sampAni;

Texture2D gTexDiffuse : register(t0);
Texture2D gTexNormal : register(t1);
Texture2D gTexPosition : register(t2);
Texture2D gTexAmbient : register(t3);
Texture2D gTexSpecular : register(t4);
Texture2D gTexTanLightPos : register(t5);
Texture2D gTexTanViewPos : register(t6);
Texture2D gTexTanTPos : register(t7);

cbuffer CAMERA_BUFFER : register(b1)
{
	float3 camPos; 
	float3 lookAt; 
	matrix viewMatrix;
}
cbuffer LIGHT_BUFFER : register(b2)
{
	float3 lightPosition;
	float3 lightColor;
	float strength;
}

struct VS_OUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

float4 main(VS_OUT input) : SV_Target
{
	float4 pOut;
	float3 normal = gTexNormal.Sample(sampAni, input.Tex).rgb;
	float3 diffuse = gTexDiffuse.Sample(sampAni, input.Tex).rgb;
	float3 position = gTexPosition.Sample(sampAni, input.Tex).rgb;
	float spec = gTexSpecular.Sample(sampAni, input.Tex).a;

if (length(normal) > 0.0f)
{
	float3 lightDir = normalize(lightPosition - position);

	float lambertian = max(dot(lightDir, normal), 0.0f);
	float3 specular = gTexSpecular.Sample(sampAni, input.Tex).rgb;

	[flatten]
	if (lambertian > 0.0f)
	{
		float3 viewDir = normalize(-lookAt);
		float3 halfDir = normalize(lightDir + lookAt);
		float specAngle = max(dot(halfDir, normal), 0.0f);
		specular = pow(specAngle, 16.0f) * spec;
	}

	float3 colorLinear = lambertian * diffuse + specular * float3(1.0f, 1.0f, 1.0f);
	pOut = float4(pow(colorLinear, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f)), 1.0f);
	return pOut;
}

pOut = float4(diffuse, 1.0f);
return pOut;
}