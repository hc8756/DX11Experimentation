#define MAX_SPECULAR_EXPONENT 256.0f

cbuffer ExternalData : register(b0)
{
	//Scene info
	float3 ambientColor;
	float padding1;
	float3 camPos;
	float padding2;
}

SamplerState BasicSamplerState : register(s0);
Texture2D DiffuseTexture : register(t0);
Texture2D SpecularTexture : register(t1);
Texture2D RoughnessTexture : register(t2);
Texture2D NormalTexture : register(t3);
Texture2D AOTexture : register(t4);

//Input
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: WORLD_POSITION;
	float3 tangent			: TANGENT;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	//Sample color and specularity values from texture
	float3 surfaceColor = DiffuseTexture.Sample(BasicSamplerState, input.uv).rgb;
	float specVal = SpecularTexture.Sample(BasicSamplerState, input.uv).r;
	float roughness = RoughnessTexture.Sample(BasicSamplerState, input.uv).r;
	float3 unpackedNormal = NormalTexture.Sample(BasicSamplerState, input.uv).rgb*2-1;
	float aoColor= saturate(pow(AOTexture.Sample(BasicSamplerState, input.uv).r,2));

	//Variables of directional light 
	float3 dirToLightD=float3(-1,0,0);
	float3 lightColorD=float3(1,1,1);
	float lightIntensityD=1.8;

	//Variables of point light 
	float3 lightPosP = float3(5, 2, -5);
	float3 dirToLightP = normalize(lightPosP - input.worldPosition);
	float3 disToLightP = distance(lightPosP, input.worldPosition);
	float3 lightColorP = float3(1, 1, 1);
	float lightIntensityP = 1.8;
	//Attenuation calculations for point light
	float lightRangeP = 8.0f; 
	float atten = lightRangeP/(disToLightP * disToLightP);

	//Scene variables
	float3 dirToCam = normalize(camPos - input.worldPosition);
	float shine = (1.0001 - roughness) * MAX_SPECULAR_EXPONENT;

	//Normal calculations
	float3 N = normalize(input.normal); 
	float3 T = normalize(input.tangent);
	T = normalize(T - N * dot(T, N)); 
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	input.normal = mul(unpackedNormal, TBN);
	
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
	float3 specularTermD = pow(RdotVD, shine) * lightColorD * lightIntensityD * specVal;
	float3 specularTermP = pow(RdotVP, shine) * lightColorP * lightIntensityP * specVal * atten;

	float3 totalColor =  surfaceColor * aoColor * (ambientTerm + diffuseTermD + diffuseTermP + specularTermD + specularTermP);
	//raised to a power for gamma correction
	return float4(pow(totalColor, 1.0f / 1.5f),1);
}