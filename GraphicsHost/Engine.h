#pragma once
#include "stdafx.h"
#include <memory>
#include <vector>
#include "Camera.h"
#include "Light.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class IDrawable;
class Camera;

class Engine
{
private:
	HWND m_hWnd;

#pragma region 
	// DirectX objects
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTarget;

	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	ComPtr<ID3D11RasterizerState> m_rasteriserState;
	D3D11_VIEWPORT m_viewport = { 0 };

	struct MatrixBuffer
	{
		Matrix modelMatrix;
		Matrix viewMatrix;
		Matrix projectionMatrix;
	};

	MatrixBuffer m_engineMatrices;
	Light m_light;

	SimpleMath::Color m_backgroundColour;
	SimpleMath::Vector3 m_rotationCentre;

	Camera m_viewCamera;

	struct VertexData
	{
		Vector3 Position;
		Vector3 Colour;
		Vector3 Normal;
	};

	ComPtr<ID3D11Buffer> m_matrixBuffer;
	ComPtr<ID3D11Buffer> m_lightBuffer;

	ComPtr<ID3D11VertexShader> m_meshVertexShader;
	ComPtr<ID3D11PixelShader> m_meshPixelShader;
	ComPtr<ID3D11InputLayout> m_meshVertexLayout;
#pragma endregion

	bool m_vsyncEnabled;

	int m_width;
	int m_height;

	Vector3 m_diffuseLightColor;

	int InitialiseDeviceAndSwapChain();
	int InitialiseRenderTarget();
	int InitialiseDepthBuffer();
	int CreateCamera();
	int CreateConstantBuffers();
	int LoadShaders();

	void SetWindowHandle(HWND hWnd);

	void Clear(const SimpleMath::Color clearColour, float clearDepth, unsigned char clearStencil);
	void Present(bool vsyncEnabled);
	void ConfigureForDrawable(const std::unique_ptr<IDrawable>& drawable);
	void SetMatricesForRender();
	void SetLightsForRender();

	void CalculateWindowDimensions(void);
	Matrix CalculateProjectionMatrix(void);

	void CreateDepthStencilView(void);

public:

	TEST_API Engine() :
		m_hWnd(),
		m_backgroundColour(Colors::Aquamarine.v),
		m_vsyncEnabled(false),
		m_viewCamera(),
		m_light()
	{
	};
	TEST_API Engine(HWND hWnd);

	int Initialise(HWND hWnd);
	void Render(const std::vector<std::unique_ptr<IDrawable>>& drawables);
	int CreateBuffer(const D3D11_BUFFER_DESC* bufferDescription, const D3D11_SUBRESOURCE_DATA* resourceData, ComPtr<ID3D11Buffer>& buffer);
	void SetBackgroundColour(Color backgroundColour);
	ComPtr<ID3D11DeviceContext> GetDeviceContext();

	void Rotate(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotationCentre(float x, float y, float z);

	void MouseZoom(POINTS oldLocation, POINTS newLocation);
	void MouseRotate(POINTS oldLocation, POINTS newLocation, float speed = 5);

	void Resize(void);
	bool IsInitialised(void);

	void SetDiffuseColor(Vector3 color);
	Vector3 GetDiffuseColor(void);

	void SetAmbientColor(Vector3 color);
	Vector3 GetAmbientColor(void);

	Ray GetIntersectionRay(int x, int y);

	TEST_API Matrix GetViewMatrix(void);
};

