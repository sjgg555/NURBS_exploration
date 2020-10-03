cbuffer MatrixBuffer : register(b0)
{
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

struct VertexShaderInput 
{
	float3 position : POSITION;
	float3 color : COLOR;
	float3 normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

VertexShaderOutput SimpleVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	matrix mvp = mul(projectionMatrix, mul(viewMatrix, modelMatrix));
	output.position = mul(mvp, float4(input.position, 1.0f));

	output.color = float4(input.color, 1.0f);

	output.normal = mul(input.normal, (float3x3)transpose(viewMatrix));
	output.normal = normalize(output.normal);

	return output;
}