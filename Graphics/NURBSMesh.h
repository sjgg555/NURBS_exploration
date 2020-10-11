#pragma once
#include "Cube.h"
#include "tinysplinecpp.h"

using namespace tinyspline;

class NURBSMesh :
	public Cube
{
public:
	NURBSMesh(int resolution, int controlPointNum, int degree);
	~NURBSMesh();

	void SetControlPoint(int index, Vector3 newPos);
	Vector3 GetControlPoint(int index);

	int GetControlPointCount(void);

	void SetResolution(int newResolution);
	int GetResolution(void);

private:
	void ConstructVertices(void);
	void ConstructIndices(void);
	void ConstructControlPoints(void);

	float m_size;

	BSpline m_edgeSpline;

	const int CONTROL_POINT_COUNT = 3;
	const int DIMENSIONS = 3;
	const int DEGREE = 3;
	const int VERTICES_PER_LAYER = 4;
	int m_layerCount;
};

