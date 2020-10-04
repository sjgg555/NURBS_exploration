#pragma once
#include "Engine.h"

class IDrawable
{
public:
	enum DrawableType
	{
		PointCloud = 0,
		Mesh = 1
	};

	virtual void Draw(Engine& renderTarget) = 0;
	virtual DrawableType GetDrawableType() = 0;

	virtual ComPtr<ID3D11Buffer> GetVertexBuffer() = 0;
	virtual ComPtr<ID3D11Buffer> GetIndexBuffer() = 0;

	virtual unsigned int GetIndexCount() = 0;
	virtual unsigned int GetVertexCount() = 0;

	virtual void SetVertexPosition(int index, Vector3 position) = 0;
	virtual Vector3 GetVertexPosition(int index) = 0;

	virtual void SetVertexColor(int index, Vector3 color) = 0;
	virtual Vector3 GetVertexColor(int index) = 0;

	virtual void SetVertexNormal(int index, Vector3 normal) = 0;
	virtual Vector3 GetVertexNormal(int index) = 0;

	virtual void SetMaterialColor(SimpleMath::Color color) = 0;
	virtual SimpleMath::Color GetMaterialColor(void) = 0;

	virtual Vector3 GetCentreOfMass(void) = 0;
	virtual BoundingBox GetBoundingBox(void) = 0;

protected:
	IDrawable(SimpleMath::Color materialColor) { m_materialColor = materialColor; }
	virtual int ConstructBuffers(Engine& renderTarget) = 0;
	bool m_isDirty = false;

	SimpleMath::Color m_materialColor;
};

