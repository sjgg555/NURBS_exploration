#include "stdafx.h"
#include "Engine.h"
#include "directxcollision.h"
#include "IDrawable.h"


Engine::Engine() :
	m_hWnd(),
	m_backgroundColour(Colors::Aquamarine.v),
	m_vsyncEnabled(false),
	m_viewCamera(),
	m_light()
{
}

Engine::Engine(HWND hWnd) :
	m_hWnd(hWnd),
	m_backgroundColour(Colors::Aquamarine.v),
	m_vsyncEnabled(false),
	m_viewCamera(),
	m_light()
{
	if (FAILED(Initialise(hWnd)))
	{
		MessageBox(0,
			TEXT("Failed to initialise engine."),
			TEXT("ERROR."),
			MB_OK);
	};
}

int Engine::Initialise(HWND hWnd)
{
	SetWindowHandle(hWnd);
	//Must initialise swa`
	//Get size of the render area from the HWND
	if (FAILED(InitialiseDeviceAndSwapChain()))
	{
		throw std::exception("Unable to initialise Device and SwapChain");
	}
	if (FAILED(InitialiseRenderTarget()))
	{
		throw std::exception("Unable to initialise Render Target");
	}
	if (FAILED(InitialiseDepthBuffer()))
	{
		throw std::exception("Unable to initialise Depth Buffer");
	}
	if (FAILED(CreateCamera()))
	{
		throw std::exception("Unable to create Camera");
	}
	if (FAILED(CreateConstantBuffers()))
	{
		throw std::exception("Unable to create Constant Buffers");
	}
	if (FAILED(LoadShaders()))
	{
		throw std::exception("Unable to load shaders");
	}

#ifdef _DEBUG
	ComPtr<ID3D11Debug> debug = nullptr;
	m_device.As(&debug);
	ComPtr<ID3D11InfoQueue> info = nullptr;
	debug.As(&info);
	info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
	info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

	debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
#endif

	Clear(m_backgroundColour, 1.0f, 0);
	Present(m_vsyncEnabled);

	return S_OK;
}

int Engine::InitialiseDeviceAndSwapChain()
{
	RECT clientRectangle;
	GetClientRect(m_hWnd, &clientRectangle);

	m_width = clientRectangle.right - clientRectangle.left;
	m_height = clientRectangle.bottom - clientRectangle.top;

	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));

	swapChainDescription.BufferDesc.Width = m_width;
	swapChainDescription.BufferDesc.Height = m_height;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.RefreshRate = { 0,1 };
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = 2;
	swapChainDescription.OutputWindow = m_hWnd;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDescription,
		&m_swapChain,
		&m_device,
		&featureLevel,
		&m_deviceContext);

	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

int Engine::InitialiseRenderTarget()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		return hr;
	}
	
	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTarget);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

int Engine::InitialiseDepthBuffer()
{
	CalculateWindowDimensions();

	CreateDepthStencilView();

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDescription;
	ZeroMemory(&depthStencilStateDescription, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDescription.DepthEnable = TRUE;
	depthStencilStateDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDescription.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDescription.StencilEnable = FALSE;

	HRESULT hr = m_device->CreateDepthStencilState(&depthStencilStateDescription, &m_depthStencilState);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_RASTERIZER_DESC rasteriserDescription;
	ZeroMemory(&rasteriserDescription, sizeof(D3D11_RASTERIZER_DESC));

	rasteriserDescription.FillMode = D3D11_FILL_WIREFRAME;
	rasteriserDescription.CullMode = D3D11_CULL_BACK;
	rasteriserDescription.FrontCounterClockwise = FALSE;
	rasteriserDescription.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
	rasteriserDescription.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	rasteriserDescription.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasteriserDescription.DepthClipEnable = TRUE;
	rasteriserDescription.ScissorEnable = FALSE;
	rasteriserDescription.MultisampleEnable = FALSE;
	rasteriserDescription.AntialiasedLineEnable = FALSE;

	hr = m_device->CreateRasterizerState(&rasteriserDescription, &m_rasteriserState);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

int Engine::CreateConstantBuffers()
{
	m_engineMatrices.modelMatrix = DirectX::XMMatrixIdentity();
	m_engineMatrices.projectionMatrix = CalculateProjectionMatrix();

	m_viewCamera.UpdateMatrix();
	Matrix result = m_viewCamera.GetViewMatrix();
	m_engineMatrices.viewMatrix = result;

	D3D11_BUFFER_DESC matrixBufferDescription;
	ZeroMemory(&matrixBufferDescription, sizeof(D3D11_BUFFER_DESC));

	matrixBufferDescription.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&matrixBufferDescription, NULL, &m_matrixBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	m_light = Light(m_device, m_deviceContext);

	return S_OK;
}

int Engine::LoadShaders()
{
	ComPtr<ID3DBlob> vertexShaderBlob;
	LPCWSTR compiledVertexShaderObject = L"VertexShader.cso";

	HRESULT hr = D3DReadFileToBlob(compiledVertexShaderObject, &vertexShaderBlob);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		nullptr,
		&m_meshVertexShader);

	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_INPUT_ELEMENT_DESC vertexLayoutDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexData, Position), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexData, Colour), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexData, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = m_device->CreateInputLayout(vertexLayoutDescription, _countof(vertexLayoutDescription), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_meshVertexLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	ComPtr<ID3DBlob> pixelShaderBlob;
	LPCWSTR compiledPixelShaderObject = L"PixelShader.cso";

	hr = D3DReadFileToBlob(compiledPixelShaderObject, &pixelShaderBlob);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_meshPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

int Engine::CreateBuffer(const D3D11_BUFFER_DESC* bufferDescription, const D3D11_SUBRESOURCE_DATA* resourceData, ComPtr<ID3D11Buffer>& buffer)
{
	return m_device->CreateBuffer(bufferDescription, resourceData, buffer.GetAddressOf());
}

void Engine::SetWindowHandle(HWND hWnd)
{
	m_hWnd = hWnd;
}

void Engine::Clear(const Color clearColour, float clearDepth, unsigned char clearStencil)
{
	m_deviceContext->ClearRenderTargetView(m_renderTarget.Get(), clearColour);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Engine::Render(const std::vector<std::unique_ptr<IDrawable>>& drawables)
{
	Clear(m_backgroundColour, 1.0f, 0);

	for (auto& item : drawables)
	{
		ConfigureForDrawable(item);
	}

	Present(m_vsyncEnabled);
}

void Engine::ConfigureForDrawable(const std::unique_ptr<IDrawable>& drawable)
{
	m_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), m_depthStencilView.Get());
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	SetMatricesForRender();
	SetLightsForRender();

	drawable->Draw(*this);

	if (drawable->GetDrawableType() == IDrawable::DrawableType::Mesh)
	{
		const UINT vertexStride = sizeof(VertexData);
		const UINT offset = 0;

		m_deviceContext->IASetVertexBuffers(0, 1, drawable->GetVertexBuffer().GetAddressOf(), &vertexStride, &offset);
		m_deviceContext->IASetInputLayout(m_meshVertexLayout.Get());
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_deviceContext->IASetIndexBuffer(drawable->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

		m_deviceContext->VSSetShader(m_meshVertexShader.Get(), nullptr, 0);

		m_deviceContext->RSSetState(m_rasteriserState.Get());
		m_deviceContext->RSSetViewports(1, &m_viewport);

		m_deviceContext->PSSetShader(m_meshPixelShader.Get(), nullptr, 0);

		m_deviceContext->DrawIndexed(drawable->GetIndexCount(), 0, 0);
	}
}

void Engine::Present(bool vSync)
{
	if (vSync)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

void Engine::SetBackgroundColour(Color backgroundColour)
{
	m_backgroundColour = backgroundColour;
}

ComPtr<ID3D11DeviceContext> Engine::GetDeviceContext()
{
	return m_deviceContext;
}

int Engine::CreateCamera()
{
	m_viewCamera.SetRotation({ 00.0f, 0.0f, 0.0f });
	m_viewCamera.SetPosition({ 0.0f, 0.0f, 1.0f });
	return S_OK;
}

void Engine::SetMatricesForRender()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	m_deviceContext->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	MatrixBuffer* dataBuf = (MatrixBuffer*)mappedSubresource.pData;

	m_viewCamera.UpdateMatrix();
	dataBuf->viewMatrix = m_viewCamera.GetViewMatrix();
	dataBuf->modelMatrix = m_engineMatrices.modelMatrix;
	dataBuf->projectionMatrix = m_engineMatrices.projectionMatrix;

	m_deviceContext->Unmap(m_matrixBuffer.Get(), 0);

	m_deviceContext->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
}

void Engine::SetLightsForRender()
{
	m_deviceContext->PSSetConstantBuffers(0, 1, m_light.GetLightBuffer().GetAddressOf());
}

void Engine::SetRotationCentre(float x, float y, float z)
{
	m_viewCamera.SetRotationCentre(Vector3(x, y, z));
}

void Engine::SetRotation(float x, float y, float z)
{
	m_viewCamera.SetRotation(Vector3(x, y, z));
}

void Engine::Rotate(float x, float y, float z)
{
	m_viewCamera.SetRotation(m_viewCamera.GetRotation() + Vector3(x, y, z));
}

void Engine::MouseRotate(POINTS oldLocation, POINTS newLocation, float speed)
{
	float xScreenDelta = float(newLocation.x - oldLocation.x);
	float yScreenDelta = float(newLocation.y - oldLocation.y);

	Vector3 newRotation(yScreenDelta, xScreenDelta, 0);
	newRotation /= speed;

	m_viewCamera.SetRotation(m_viewCamera.GetRotation() + newRotation);
}

void Engine::MouseZoom(POINTS oldLocation, POINTS newLocation)
{
	float xSpeed = float(newLocation.x - oldLocation.x);
	float ySpeed = float(newLocation.y - oldLocation.y);

	Vector3 cameraLocation = m_viewCamera.GetPosition();
	Vector3 lookDirection = m_viewCamera.GetRotationCentre() - cameraLocation;
	lookDirection = lookDirection * (ySpeed / 1000);
	m_viewCamera.SetPosition(cameraLocation + lookDirection);
}

void Engine::Resize(void)
{
	m_deviceContext->ClearState();
	m_deviceContext->OMSetRenderTargets(0, 0, 0);

	m_depthStencilView->Release();
	m_renderTarget->Release();

	HRESULT hr;
	hr = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	if (FAILED(hr))
	{
		throw std::exception("Unable to resize swapchain buffers");
	}

	ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		throw std::exception("Unable to get swapchain back buffer");
	}
	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTarget.GetAddressOf());

	if (FAILED(hr))
	{
		throw std::exception("Unable to Create render target");
	}

	CalculateWindowDimensions();
	CreateDepthStencilView();

	m_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), m_depthStencilView.Get());
	m_deviceContext->RSSetViewports(1, &m_viewport);
}

void Engine::CalculateWindowDimensions(void)
{
	RECT clientRectangle;
	GetClientRect(m_hWnd, &clientRectangle);

	m_width = clientRectangle.right - clientRectangle.left;
	m_height = clientRectangle.bottom - clientRectangle.top;

	m_viewport.Width = static_cast<float>(m_width);
	m_viewport.Height = static_cast<float>(m_height);
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}

void Engine::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depthStencilDescription;
	ZeroMemory(&depthStencilDescription, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilDescription.Width = m_width;
	depthStencilDescription.Height = m_height;
	depthStencilDescription.MipLevels = 1;
	depthStencilDescription.ArraySize = 1;
	depthStencilDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDescription.SampleDesc.Count = 1;
	depthStencilDescription.SampleDesc.Quality = 0;
	depthStencilDescription.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDescription.CPUAccessFlags = 0;

	ComPtr<ID3D11Texture2D> depthBuffer;
	HRESULT hr = m_device->CreateTexture2D(&depthStencilDescription, nullptr, depthBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		throw std::exception("Unable to create Depth Buffer");
	}

	hr = m_device->CreateDepthStencilView(depthBuffer.Get(), nullptr, m_depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		throw std::exception("Unable to Create Depth Stencil View");
	}

	m_engineMatrices.projectionMatrix = CalculateProjectionMatrix();
}

bool Engine::IsInitialised()
{
	return (m_device != NULL);
}

Matrix Engine::CalculateProjectionMatrix()
{
	return DirectX::XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), float(m_width) / float(m_height), 0.1f, 1000.0f);
}

void Engine::SetDiffuseColor(Vector3 color)
{
	m_light.SetDiffuse(color);
}

Vector3 Engine::GetDiffuseColor(void)
{
	return m_light.GetDiffuse();
}

void Engine::SetAmbientColor(Vector3 color)
{
	m_light.SetAmbient(color);
}

Vector3 Engine::GetAmbientColor(void)
{
	return m_light.GetAmbient();
}

Ray Engine::GetIntersectionRay(int x, int y)
{
	float pointX = (2.0f * static_cast<float>(x) / static_cast<float>(m_width)) - 1.0f;
	float pointY = (2.0f * static_cast<float>(y) / static_cast<float>(m_height)) - 2.0f;

	Matrix projectionMatrix = m_engineMatrices.projectionMatrix;
	pointX = pointX / projectionMatrix._11;
	pointY = pointY / projectionMatrix._22;

	Matrix inverseViewMatrix = m_engineMatrices.viewMatrix.Invert();
	
	Vector3 rayDirection;
	rayDirection.x = (pointX * inverseViewMatrix._11) + (pointY * inverseViewMatrix._21) + inverseViewMatrix._31;
	rayDirection.y = (pointX * inverseViewMatrix._12) + (pointY * inverseViewMatrix._22) + inverseViewMatrix._32;
	rayDirection.z = (pointX * inverseViewMatrix._13) + (pointY * inverseViewMatrix._23) + inverseViewMatrix._33;

	Vector3 origin = m_viewCamera.GetPosition();

	Matrix inverseModelMatrix = m_engineMatrices.modelMatrix.Invert();

	Vector3 rayOrigin;
	rayOrigin = Vector3::Transform(rayOrigin, inverseModelMatrix);
	rayDirection = Vector3::TransformNormal(rayDirection, inverseModelMatrix);

	rayDirection.Normalize();

	return Ray(rayOrigin, rayDirection);
}