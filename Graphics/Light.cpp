#include "pch.h"
#include "Light.h"

Light::Light(ComPtr<ID3D11Device> device, 
	ComPtr<ID3D11DeviceContext> context, 
	Vector3 diffuseColor,
	Vector3 ambientColor,
	Vector3 direction) 
	:
	m_diffuseColor(diffuseColor), 
	m_ambientColor(ambientColor),
	m_direction(direction),
	m_context(context)
{
	D3D11_BUFFER_DESC lightBufferDescription;
	ZeroMemory(&lightBufferDescription, sizeof(D3D11_BUFFER_DESC));

	lightBufferDescription.ByteWidth = sizeof(LightBuffer);
	lightBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device->CreateBuffer(&lightBufferDescription, NULL, &m_lightBuffer);
	if (FAILED(hr))
	{
		return;
	}

	FillBuffer();
}

void Light::SetDiffuse(Vector3 color)
{
	m_diffuseColor = color;
	FillBuffer();
}

Vector3 Light::GetDiffuse(void)
{
	return m_diffuseColor;
}

void Light::SetAmbient(Vector3 color)
{
	m_ambientColor = color;
	FillBuffer();
}

Vector3 Light::GetAmbient(void)
{
	return m_ambientColor;
}

void Light::SetDirection(Vector3 direction)
{
	m_direction = direction;
	FillBuffer();
}

Vector3 Light::GetDirection(void)
{
	return m_direction;
}


ComPtr<ID3D11Buffer> Light::GetLightBuffer(void)
{
	return m_lightBuffer;
}

void Light::FillBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	m_context->Map(m_lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	LightBuffer* dataBuf = (LightBuffer*)mappedSubresource.pData;

	dataBuf->diffuseColor = m_diffuseColor;
	dataBuf->padding = 0.0f;
	dataBuf->ambientColor = m_ambientColor;
	dataBuf->padding1 = 0.0f;
	dataBuf->lightDirection = m_direction;
	dataBuf->padding2 = 0.0f;

	m_context->Unmap(m_lightBuffer.Get(), 0);
}