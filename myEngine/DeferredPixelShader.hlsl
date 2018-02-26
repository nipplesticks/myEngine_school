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
	float4 camPos; 
	float4 lookAt; 
	matrix viewMatrix;
}
cbuffer LIGHT_BUFFER : register(b5)
{
	float4 lightPosition;
	float4 lightColor;
	float4 lightDir;
	matrix lightProj;
	matrix lightView;
	float strength;
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
		
		// Shadow stuf/
		float4 posLightH = mul(float4(position, 1), lightView);
		posLightH = mul(posLightH, lightProj);
		posLightH.xy /= posLightH.w;

		if (abs(posLightH.x) > 1.0f)
			return float4(amb, 1);
		if (abs(posLightH.y) > 1.0f)
			return float4(amb, 1);


		float2 smTex = float2(0.5f * posLightH.x + 0.5f, -0.5f * posLightH.y + 0.5f);
		float depth = posLightH.z / posLightH.w;
		float SHADOW_EPSILON = 0.001f;
		float shadowCoeff = (gTexShadowMap.Sample(sampAni, smTex).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
		float3 s = gTexShadowMap.Sample(sampAni, smTex).rgb;
		
		float3 viewer = normalize(camPos.xyz - position);
		float3 lightDirToObject = normalize(lightPosition.xyz - position);

		float3 diffuse = diffuseColor * max(dot(normal, lightDirToObject), 0.0);
		float3 halfwayDir = normalize(lightDirToObject + viewer);

		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
		float3 specular = lightColor.rgb * spec * specLevel;
		return float4(amb + (specular + diffuse) * shadowCoeff, 1.0);
}