/*Common data structures used in shaders:
Scalar: int, bool, float
Vector: float2, float3, float4 (16 bytes)
Matrix: float3x3, float4x4 (matrix)*/

//Shader cbuffer should match the constant buffer of information being passed into it
//Remember that buffer data must start at 16-byte mark
cbuffer ExternalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

//This struct represents a vertex we are getting from pipeline (input)
//This struct should match our definition of a vertex 
struct VertexShaderInput
{
	float3 localPosition	: POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
};

//This struct represents information we are sending to pixel shader (output)
//This should match the VertexToPixel struct in the pixel shader
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: WORLD_POSITION;
	float3 tangent			: TANGENT;
};

VertexToPixel main(VertexShaderInput input)
{
	//Set up output struct
	VertexToPixel output;

	//Calculate screen position & tangent of this vertex after transformations
	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	output.tangent = mul(wvp, float4(input.tangent, 1.0f));

	//Pass other input data through 
	output.uv = input.uv;
	output.normal = mul((float3x3)wvp, input.normal); 
	output.normal = normalize(output.normal);//normal is transformed & normalized for lighting
	output.worldPosition = mul(world, float4(input.localPosition, 1.0f)).xyz;

	return output;
}