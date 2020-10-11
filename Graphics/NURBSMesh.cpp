#include "pch.h"
#include "NURBSMesh.h"


NURBSMesh::NURBSMesh(int resolution, int controlPointNum, int degree) : Cube()
, m_size(1.0f)
, DIMENSIONS(3)
, DEGREE(degree)
, CONTROL_POINT_COUNT(controlPointNum)
, m_edgeSpline(controlPointNum, 3, degree)
, m_layerCount(resolution)
{
	ConstructControlPoints();
	ConstructVertices();
	ConstructIndices();

	m_isDirty = true;
}

NURBSMesh::~NURBSMesh()
{
}

void NURBSMesh::ConstructControlPoints(void)
{
	float controlPointSpacing = 2 * m_size / (CONTROL_POINT_COUNT - 1);

	std::vector<real> controlPoints = m_edgeSpline.controlPoints();

	for (size_t i = 0; i < controlPoints.size(); i += DIMENSIONS)
	{
		float zSpacing = (controlPointSpacing * (i / DIMENSIONS));
		controlPoints[i] = m_size;
		controlPoints[i + 1] = m_size;
		controlPoints[i + 2] = static_cast<real>(m_size) - static_cast<real>(zSpacing);
	}

	m_edgeSpline.setControlPoints(controlPoints);

}


void NURBSMesh::ConstructVertices(void)
{
	m_vertices.clear();

	float vertexSpacing = 2 * m_size / (m_layerCount);

	try
	{
		for (int i = 0; i < m_layerCount + 1; i++)
		{
			float normalisedU = (i * vertexSpacing) / 2 * m_size;
			std::vector<real> result = m_edgeSpline.eval(normalisedU).result();

			VertexLayout vertex;
			vertex.Color = Vector3(m_materialColor.R(), m_materialColor.G(), m_materialColor.B());
			vertex.Position = Vector3(result[0], result[1], result[2]);
			vertex.Normal = vertex.Position;

			m_vertices.push_back(vertex);

			vertex.Position = Vector3(-result[0], result[1], result[2]);
			vertex.Normal = vertex.Position;

			m_vertices.push_back(vertex);

			vertex.Position = Vector3(-result[0], -result[1], result[2]);
			vertex.Normal = vertex.Position;

			m_vertices.push_back(vertex);

			vertex.Position = Vector3(result[0], -result[1], result[2]);
			vertex.Normal = vertex.Position;

			m_vertices.push_back(vertex);
		}
	}
	catch (const std::runtime_error& ex)
	{
		MessageBox(NULL, (LPCWSTR)ex.what(), (LPCWSTR)"", NULL);
	}
}

void NURBSMesh::ConstructIndices(void)
{
	m_indices =
	{
		0, 1, 2,
		0, 2, 3,
	};

	for (int i = 0; i < m_vertices.size() - 4; i++)
	{
		int high;
		if ((i % VERTICES_PER_LAYER) != 3)
		{
			high = (i + VERTICES_PER_LAYER + 1);
			m_indices.push_back(i);
			m_indices.push_back(high);
			m_indices.push_back(i + 1);

			m_indices.push_back(i);
			m_indices.push_back(high - 1);
			m_indices.push_back(high);
		}
	}

	for (int i = 0; i < m_layerCount; i++)
	{
		int start = (i * VERTICES_PER_LAYER + 3);
		m_indices.push_back(start);
		m_indices.push_back(start + 1);
		m_indices.push_back(i * VERTICES_PER_LAYER);

		m_indices.push_back(start);
		m_indices.push_back(start + VERTICES_PER_LAYER);
		m_indices.push_back(start + 1);
	}

	int end = VERTICES_PER_LAYER * (m_layerCount);
	m_indices.push_back(end);
	m_indices.push_back(end + 2);
	m_indices.push_back(end + 1);
	m_indices.push_back(end);
	m_indices.push_back(end + 3);
	m_indices.push_back(end + 2);
}

void NURBSMesh::SetControlPoint(int index, Vector3 newPos)
{
	std::vector<real> newControlPoint = { newPos.x, newPos.y, newPos.z };
	m_edgeSpline.setControlPointAt(index, newControlPoint);
	ConstructVertices();
	m_isDirty = true;
}

Vector3 NURBSMesh::GetControlPoint(int index)
{
	auto controlPoint = m_edgeSpline.controlPointAt(index);
	return Vector3(controlPoint[0], controlPoint[1], controlPoint[2]);
}

int NURBSMesh::GetControlPointCount(void)
{
	return CONTROL_POINT_COUNT;
}

int NURBSMesh::GetResolution(void)
{
	return m_layerCount;
}

void NURBSMesh::SetResolution(int newResolution)
{
	if (newResolution > 0)
	{
		m_layerCount = newResolution;

		ConstructVertices();
		ConstructIndices();

		m_isDirty = true;
	}
}
