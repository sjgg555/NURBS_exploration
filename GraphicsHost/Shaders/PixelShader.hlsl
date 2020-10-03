cbuffer LightBuffer
{
	float3 diffuseColor;
	float3 ambientColor;
	float3 lightDirection;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

float3 SimplePixelShader(PixelShaderInput input) : SV_TARGET
{
	float lightBrightness = 1.0f;
	float3 lightDir = -lightDirection;
	float4 materialColor = input.color;
	float3 normal = input.normal;
	float3 color;

	color = ambientColor;

	lightBrightness = saturate(dot(normal, lightDir));

	if (lightBrightness > 0.0f)
	{
		color += (diffuseColor * lightBrightness);
	}

	color = saturate(color);

	return (materialColor * color);
}