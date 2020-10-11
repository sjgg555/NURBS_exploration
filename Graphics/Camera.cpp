#include "pch.h"
#include "Camera.h"


void Camera::SetRotation(Vector3 rotation)
{
	if (rotation.x > MAX_X_ROTATION)
	{
		rotation.x = (MAX_X_ROTATION - 0.001f);
	}
	else if (rotation.x < -MAX_X_ROTATION)
	{
		rotation.x = (-MAX_X_ROTATION + 0.001f);
	}

	m_rotation = rotation;
}

void Camera::SetPosition(Vector3 position)
{
	m_cameraPosition = position;
}

void Camera::SetRotationCentre(Vector3 rotationCentre)
{
	m_rotationCentre = rotationCentre;
}

Vector3 Camera::GetRotation()
{
	return m_rotation;
}

Vector3 Camera::GetPosition()
{
	return m_cameraPosition;
}

Vector3 Camera::GetRotationCentre()
{
	return m_rotationCentre;
}

void Camera::UpdateMatrix()
{
	const float yaw(m_rotation.y * DEGREES_TO_RADIANS);
	const float pitch(m_rotation.x * DEGREES_TO_RADIANS);
	const float roll(m_rotation.z * DEGREES_TO_RADIANS);

	Matrix currentMatrix = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);

	//Need to translate the lookDirection to the rotation centre, then move it back after rotated
	Vector3 cameraTranslation = m_rotationCentre - m_cameraPosition;
	cameraTranslation = DirectX::XMVector3TransformCoord(cameraTranslation, currentMatrix);
	Vector3 newCameraPosition = m_rotationCentre + cameraTranslation;

	Vector3 upDirection(0.0f, 1.0f, 0.0f);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(newCameraPosition, m_rotationCentre, upDirection);
}

Matrix Camera::GetViewMatrix()
{
	return m_viewMatrix;
}