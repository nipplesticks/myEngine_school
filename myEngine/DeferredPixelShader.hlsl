SamplerState sampAni;

Texture2D gTexDiffuse : register(t0);
Texture2D gTexNormal : register(t1);
Texture2D gTexPosition : register(t2);
Texture2D gTexAmbient : register(t3);
Texture2D gTexSpecular : register(t4);
Texture2D gTexShadowMap : register(t5);

Texture2D gTexAim : register(t10);
Texture2D gTexWin : register(t11);

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
	matrix lightDir;
	matrix lightProj;
	matrix lightView;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 main(VS_OUT input) : SV_Target
{
	if (gTexWin.Sample(sampAni, input.Tex).a != 0)
		return float4(gTexWin.Sample(sampAni, input.Tex).rgb, 1);
	if (gTexAim.Sample(sampAni, input.Tex).a != 0)
		return float4(gTexAim.Sample(sampAni, input.Tex).rgb, 1);

		float4 pOut;
		float3 normal = gTexNormal.Sample(sampAni, input.Tex).rgb;

		float3 diffuseColor = gTexDiffuse.Sample(sampAni, input.Tex).rgb;
		if (length(normal) == 0)
		{
			return float4(diffuseColor, 1.0f);
		}

		float3 position = gTexPosition.Sample(sampAni, input.Tex).rgb;
		float specLevel = gTexSpecular.Sample(sampAni, input.Tex).r;
		float3 amb = gTexAmbient.Sample(sampAni, input.Tex).rgb;

		if (length(amb) == 0)
			amb = diffuseColor * 0.2;
		float3 viewer = normalize(camPos - position);
		float3 lightDir = normalize(lightPosition - position);

		float3 diffuse = amb + diffuseColor * max(dot(normal, lightDir), 0.0);
		float3 halfwayDir = normalize(lightDir + viewer);

		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
		float3 specular = lightColor * spec * specLevel;
		return float4(specular + diffuse, 1.0);
}