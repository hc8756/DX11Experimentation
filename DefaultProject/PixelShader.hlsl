#define MAX_SPECULAR_EXPONENT 256.0f

cbuffer ExternalData : register(b0)
{
	//Material info
	float4 colorTint;
	float roughness;
	//Scene info
	float3 ambientColor;
	float3 camPos;
}

//Input
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: WORLD_POSITION;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	//Variables of directional light 
	float3 dirToLightD=float3(-1,0,0);
	float3 lightColorD=float3(1,1,1);
	float lightIntensityD=1.0;

	//Variables of point light 
	float3 lightPosP = float3(5, 2, 0);
	float3 dirToLightP = normalize(lightPosP - input.worldPosition);
	float3 disToLightP = distance(lightPosP, input.worldPosition);
	float3 lightColorP = float3(1, 1, 1);
	float lightIntensityP = 0.5;
	//Attenuation calculations for point light
	float lightRangeP = 5.0f; 
	float atten = lightRangeP/(disToLightP* disToLightP);

	//Scene variables
	float3 dirToCam = normalize(camPos - input.worldPosition);
	float shine = (1 - roughness) * MAX_SPECULAR_EXPONENT;
	input.normal = normalize(input.normal);//re-normalize interpolated normals
	
	/*Lighting equations*/
	//Ambient term 
	float3 ambientTerm = ambientColor;

	//Diffuse term (Directional)
	float3 diffuseTermD = saturate(dot(input.normal, dirToLightD)) * lightColorD * lightIntensityD;
	//Diffuse term (Point)
	float3 diffuseTermP = saturate(dot(input.normal, dirToLightP)) * lightColorP * lightIntensityP * atten;
	
	//Specular term (Directional)
	float3 reflectDirD = reflect(-dirToLightD, input.normal);
	float3 reflectDirP = reflect(-dirToLightP, input.normal);
	float RdotVD = saturate(dot(reflectDirD, dirToCam));
	float RdotVP = saturate(dot(reflectDirP, dirToCam));
	float3 specularTermD = pow(RdotVD, shine) * lightColorD * lightIntensityD;
	float3 specularTermP = pow(RdotVP, shine) * lightColorP * lightIntensityP * atten;

	return float4(specularTermD+specularTermP+colorTint*(ambientTerm+diffuseTermD+diffuseTermP),1);
}