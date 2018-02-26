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
	//<HUD>
	// If pixel is not transparent, return the pixel color
	if (gTexWin.Sample(sampAni, input.Tex).a != 0)
		return float4(gTexWin.Sample(sampAni, input.Tex).rgb, 1);
	if (gTexAim.Sample(sampAni, input.Tex).a != 0)
		return float4(gTexAim.Sample(sampAni, input.Tex).rgb, 1);
	//</HUD>

		float3 normal = gTexNormal.Sample(sampAni, input.Tex).rgb;
		float3 diffuseColor = gTexDiffuse.Sample(sampAni, input.Tex).rgb;

		// If an object don't have a normal, return the pixel color
		if (length(normal) == 0)
		{
			return float4(diffuseColor, 1.0f);
		}

		diffuseColor *= lightColor.rgb;

		float3 position = gTexPosition.Sample(sampAni, input.Tex).rgb;
		float specLevel = gTexSpecular.Sample(sampAni, input.Tex).r;
		float3 amb = gTexAmbient.Sample(sampAni, input.Tex).rgb;

		// if no ambient texture, create ambient from diffuseColor
		if (length(amb) == 0)
			amb = diffuseColor * 0.2;
		
		// <Shadow stuff>
		float4 posLightH = mul(float4(position, 1), lightView); // Translate the world position into the view space of the light
		posLightH = mul(posLightH, lightProj);					// Translate the view position into the projection space of the light
		posLightH.xy /= posLightH.w;							// Get the texture coords of the "object" in the shadow map
		if (abs(posLightH.x) > 1.0f)							// Check if we are outside the shadow map (we are not in the light)
			return float4(amb, 1);
		if (abs(posLightH.y) > 1.0f)							// Check if we are outside the shadow map (we are not in the light)
			return float4(amb, 1);
		float2 smTex = float2(0.5f * posLightH.x + 0.5f, -0.5f * posLightH.y + 0.5f);	// Texcoords are not [-1, 1], change the coords to [0, 1]
		float depth = posLightH.z / posLightH.w;										// Get the actual depth (seen from the camera)
		float SHADOW_EPSILON = 0.001f;													// Some small value
		float shadowCoeff = (gTexShadowMap.Sample(sampAni, smTex).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;	// If the depth from camera is larger than depth from light,
		// </Shadow stuff>																						// something is in occluding (in the way) and we will multiply the final color with 0, else 1;


		// Ambient, diffuse & specular phonh
		float3 viewer = normalize(camPos.xyz - position);							// Vector from position to camera
		float3 lightDirToObject = normalize(lightPosition.xyz - position);			// The light dir from position to light

		float3 diffuse = diffuseColor * max(dot(normal, lightDirToObject), 0.0);	//calculate the diffuse factor
		
		
		float3 halfwayDir = normalize(lightDirToObject + viewer); // Create a vector to use to get the specular level

		
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);	// Calculate the "size" of the shineness-reflection of the pixel
		
		// final specular
		float3 specular = spec * specLevel;
		return float4(amb + (specular + diffuse) * shadowCoeff, 1.0);
}