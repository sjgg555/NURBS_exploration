#pragma once

using namespace DirectX::SimpleMath;

class Light
{
private:
	Vector3 m_diffuseColor;
	Vector3 m_ambientColor;
	Vector3 m_direction;

	ComPtr<ID3D11Buffer> m_lightBuffer;
	ComPtr<ID3D11DeviceContext> m_context;

	void FillBuffer();

public:
	Light() {};
	Light(ComPtr<ID3D11Device> device, 
		ComPtr<ID3D11DeviceContext> context, 
		Vector3 diffuseLightColor = {1.0f, 1.0f, 1.0f},
		Vector3 ambientColor = {0.4f, 0.4f, 0.4f},
		Vector3 lightDirection = Vector3(0, 0, 1.0f));
	~Light() {};

	void SetDiffuse(Vector3 color);
	Vector3 GetDiffuse(void);

	void SetAmbient(Vector3 color);
	Vector3 GetAmbient(void);


	void SetDirection(Vector3 direction);
	Vector3 GetDirection(void);

	ComPtr<ID3D11Buffer> GetLightBuffer(void);

	struct LightBuffer
	{
		Vector3 diffuseColor;
		float padding;
		Vector3 ambientColor;
		float padding1;
		Vector3 lightDirection;
		float padding2;
	};

};

