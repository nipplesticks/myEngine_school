SamplerState sampAni;

Texture2D gTexDiffuse : register(t0);
Texture2D gTexNormal : register(t1);
Texture2D gTexPosition : register(t2);
Texture2D gTexAmbient : register(t3);
Texture2D gTexSpecular : register(t4);
Texture2D gTexLightPos : register(t5);
Texture2D gTexLightDir : register(t6);
Texture2D gTexViewer : register(t7);

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
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 main(VS_OUT input) : SV_Target
{
	float4 pOut;
	float3 normal = gTexNormal.Sample(sampAni, input.Tex).rgb;
	float3 diffuseColor = gTexDiffuse.Sample(sampAni, input.Tex).rgb;
	float3 position = gTexPosition.Sample(sampAni, input.Tex).rgb;
	float specLevel = gTexSpecular.Sample(sampAni, input.Tex).r;
	float3 amb = gTexAmbient.Sample(sampAni, input.Tex).rgb;
	
	float3 lightPos = gTexLightPos.Sample(sampAni, input.Tex).rgb;
	float3 lightDir = gTexLightDir.Sample(sampAni, input.Tex).rgb;
	float3 viewer = gTexViewer.Sample(sampAni, input.Tex).rgb;

    if (length(normal) > 0.0f)
    {
	    float3 lightDir = normalize(lightPosition - position);
	    float lambertian = max(dot(lightDir, normal), 0.0f);
	    float specular = 0;
	    [flatten]
	    if (lambertian > 0.0f)
	    {
		    float3 halfDir = normalize(lightDir + viewer);
		    float specAngle = max(dot(halfDir, normal), 0.0f);
		    specular = pow(specAngle, 16) * specLevel;
	    }

        float3 colorLinear =  strength * lightColor * (lambertian * diffuseColor + specular);
	    pOut = float4(0.2 * diffuseColor, 0) + float4(pow(colorLinear, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f)), 1.0f);
	    return pOut;
    }

pOut = float4(diffuseColor, 1.0f);
return pOut;
}