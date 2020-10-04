#pragma once
#include "IDrawable.h"

class Cube :
	public IDrawable
{
public:
	Cube();
	~Cube();

	void Draw(Engine& renderTarget);
	DrawableType GetDrawableType() { return DrawableType::Mesh; };
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();

	unsigned int GetIndexCount();
	unsigned int GetVertexCount();

	void Translate(double x, double y, double z);
	void SetMaterialColor(SimpleMath::Color materialColor);
	SimpleMath::Color GetMaterialColor(void);

	void SetVertexPosition(int index, Vector3 position);
	Vector3 GetVertexPosition(int index);

	void SetVertexColor(int index, Vector3 color);
	Vector3 GetVertexColor(int index);

	void SetVertexNormal(int index, Vector3 normal);
	Vector3 GetVertexNormal(int index);
	
	Vector3 GetCentreOfMass(void);
	BoundingBox GetBoundingBox(void);

protected:
	int ConstructBuffers(Engine& renderTarget);

	struct VertexLayout
	{
		Vector3 Position;
		Vector3 Color;
		Vector3 Normal;
	};

	std::vector<VertexLayout> m_vertices =
	{
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, -1.0f, -1.0f)}, // 0
		{ Vector3(-1.0f,  1.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(-1.0f,  1.0f, -1.0f)}, // 1
		{ Vector3(1.0f,  1.0f, -1.0f), Vector3(1.0f, 1.0f, 0.0f), Vector3(1.0f,  1.0f, -1.0f)}, // 2
		{ Vector3(1.0f, -1.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, -1.0f, -1.0f)}, // 3
		{ Vector3(-1.0f, -1.0f,  1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, -1.0f,  1.0f)}, // 4
		{ Vector3(-1.0f,  1.0f,  1.0f), Vector3(0.0f, 1.0f, 1.0f), Vector3(-1.0f,  1.0f,  1.0f)}, // 5
		{ Vector3(1.0f,  1.0f,  1.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f,  1.0f,  1.0f)}, // 6
		{ Vector3(1.0f, -1.0f,  1.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, -1.0f,  1.0f)}  // 7
	};

	std::vector<WORD> m_indices =
	{
		0, 1, 2,
		0, 2, 3,
		4, 6, 5,
		4, 7, 6,
		4, 5, 1,
		4, 1, 0,
		3, 2, 6,
		3, 6, 7,
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7
	};

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
};

