#pragma once

using namespace DirectX::SimpleMath;

class Camera
{
public:
	TEST_API Camera() {};
	~Camera() {};

	void SetRotation(Vector3 rotation);
	void SetPosition(Vector3 translation);
	void SetRotationCentre(Vector3 rotatioCentre);

	Vector3 GetPosition();
	Vector3 GetRotation();
	Vector3 GetRotationCentre();

	void UpdateMatrix();
	Matrix GetViewMatrix();

private:
	Vector3 m_cameraPosition;
	Vector3 m_rotation;
	Matrix m_viewMatrix;
	Vector3 m_rotationCentre;
	Quaternion m_totalRotation;

	const float DEGREES_TO_RADIANS = 0.0174532925f;
	const float MAX_X_ROTATION = 90;
	const float MAX_Y_ROTATION = 360;
};

