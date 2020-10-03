#include "stdafx.h"
#include "Cube.h"

Cube::Cube() : IDrawable(SimpleMath::Color(Colors::DarkBlue.v))
{
	m_isDirty = true;
}


Cube::~Cube()
{
}

void Cube::Draw(Engine& renderTarget)
{
	ConstructBuffers(renderTarget);
}

int Cube::ConstructBuffers(Engine& renderTarget)
{
	if (m_isDirty && !m_vertices.empty())
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(VertexLayout) * (int)m_vertices.size();
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA resData;
		ZeroMemory(&resData, sizeof(D3D11_SUBRESOURCE_DATA));

		resData.pSysMem = m_vertices.data();

		HRESULT hr = renderTarget.CreateBuffer(&vertexBufferDesc, &resData, m_vertexBuffer);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	if (m_isDirty && !m_indices.empty())
	{
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(WORD) * (int)m_indices.size();
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA resData;
		ZeroMemory(&resData, sizeof(D3D11_SUBRESOURCE_DATA));

		resData.pSysMem = m_indices.data();

		HRESULT hr = renderTarget.CreateBuffer(&indexBufferDesc, &resData, m_indexBuffer);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	m_isDirty = false;
	return S_OK;
}

ComPtr<ID3D11Buffer> Cube::GetVertexBuffer()
{
	return m_vertexBuffer;
}

ComPtr<ID3D11Buffer> Cube::GetIndexBuffer()
{
	return m_indexBuffer;
}

unsigned int Cube::GetVertexCount()
{
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned int Cube::GetIndexCount()
{
	return static_cast<unsigned int>(m_indices.size());
}

void Cube::Translate(double x, double y, double z)
{
	for (auto& vertex : m_vertices)
	{
		vertex.Position.x += (float)x;
		vertex.Position.y += (float)y;
		vertex.Position.z += (float)z;
	}

	m_isDirty = true;
}

void Cube::SetMaterialColor(SimpleMath::Color materialColor)
{
	m_materialColor = materialColor;

	for (auto& vertex : m_vertices)
	{
		vertex.Color.x = m_materialColor.R();
		vertex.Color.y = m_materialColor.G();
		vertex.Color.z = m_materialColor.B();
	}

	m_isDirty = true;
}

void Cube::SetVertexPosition(int index, Vector3 position)
{
	m_vertices[index].Position = position;

	m_isDirty = true;
}

void Cube::SetVertexColor(int index, Vector3 color)
{
	m_vertices[index].Color = color;

	m_isDirty = true;
}

void Cube::SetVertexNormal(int index, Vector3 normal)
{
	m_vertices[index].Normal = normal;

	m_isDirty = true;
}

Vector3 Cube::GetVertexPosition(int index)
{
	return m_vertices[index].Position;
}

Vector3 Cube::GetVertexColor(int index)
{
	return m_vertices[index].Color;
}

Vector3 Cube::GetVertexNormal(int index)
{
	return m_vertices[index].Normal;
}

SimpleMath::Color Cube::GetMaterialColor(void)
{
	return m_materialColor;
}