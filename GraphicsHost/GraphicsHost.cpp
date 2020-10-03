// GraphicsHost.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GraphicsHost.h"
#include <algorithm>
#include <vector>
#include "IDrawable.h"
#include "Engine.h"
#include "Cube.h"
#include <sstream>
#include "NURBSMesh.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

Engine m_engine;
std::vector<std::unique_ptr<IDrawable>> m_geometries;
POINTS m_mousePressedLocation;
POINTS m_mouseLocation;

bool m_mouseLeftPressed = false;
bool m_middleMousePressed = false;

int m_currentColor = 0;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GRAPHICSHOST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICSHOST));

	MSG msg = { 0 };

	// Create a mesh to draw, spline has a degree of 2 and 3 control points
	//with 15 vertices along its edges
	int resolution = 15;
	int curveDegree = 2;
	int controlPointNum = 3;

	NURBSMesh mesh(resolution, controlPointNum, curveDegree);

	m_geometries.push_back(std::make_unique<NURBSMesh>(mesh));

	m_engine.SetDiffuseColor(Vector3(0.7f, 0.7f, 0.7f));

	m_engine.Render(m_geometries);

	bool render = true;
	// Main message loop:
	while (msg.message != WM_QUIT)
	{
		if (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

// Defines of the window to render to
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICSHOST));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPHICSHOST);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

// Initilises any parts of the window and creates the engine
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	try
	{
		m_engine.Initialise(hWnd);
	}
	catch (const std::exception& ex)
	{
		MessageBox(NULL, ex.what(), "ERROR", MB_OK | MB_ICONERROR);
	}
	m_engine.SetBackgroundColour({ 1.0, 1.0, 1.0 });

	return TRUE;
}

// Handles windows messages that have been created while the window has focus.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_mouseLeftPressed = true;
		m_mousePressedLocation = MAKEPOINTS(lParam);
		break;
	}
	case WM_LBUTTONUP:
	{
		m_mouseLeftPressed = false;
		break;
	}
	case WM_MBUTTONDOWN:
	{
		m_middleMousePressed = true;
		m_mousePressedLocation = MAKEPOINTS(lParam);
		break;
	}
	case WM_MBUTTONUP:
	{
		m_middleMousePressed = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		// Handles capturing mouse movements across the viewport 
		m_mouseLocation = MAKEPOINTS(lParam);

		if (m_mouseLeftPressed)
		{
			m_engine.MouseRotate(m_mousePressedLocation, m_mouseLocation);
		}
		else if (m_middleMousePressed)
		{
			m_engine.MouseZoom(m_mousePressedLocation, m_mouseLocation);
		}

		if (m_mouseLeftPressed || m_middleMousePressed)
		{
			m_mousePressedLocation = m_mouseLocation;
			m_engine.Render(m_geometries);
		}
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
		{
			// Moves the edge splines middle control point along in +ve X
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			int index = (mesh->GetControlPointCount() - 1) / 2;
			Vector3 ctrlPoint = mesh->GetControlPoint(index);
			ctrlPoint.x += 0.5f;
			mesh->SetControlPoint(index, ctrlPoint);
			break;
		}
		case VK_RIGHT:
		{
			// Moves the edge splines middle control point along in -ve X
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			int index = (mesh->GetControlPointCount() - 1) / 2;
			Vector3 ctrlPoint = mesh->GetControlPoint(index);
			ctrlPoint.x -= 0.5f;
			mesh->SetControlPoint(index, ctrlPoint);
			break;
		}
		case VK_UP:
		{
			// Moves the edge splines middle control point along in +ve Y
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			int index = (mesh->GetControlPointCount() - 1) / 2;
			Vector3 ctrlPoint = mesh->GetControlPoint(index);
			ctrlPoint.y += 0.5f;
			mesh->SetControlPoint(index, ctrlPoint);
			break;
		}
		case VK_DOWN:
		{
			// Moves the edge splines middle control point along in -ve Y
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			int index = (mesh->GetControlPointCount() - 1) / 2;
			Vector3 ctrlPoint = mesh->GetControlPoint(index);
			ctrlPoint.y -= 0.5f;
			mesh->SetControlPoint(index, ctrlPoint);
			break;
		}
		case VK_PRIOR:
		{
			// Moves the edge splines middle control point along in +ve Z
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			int index = (mesh->GetControlPointCount() - 1) / 2;
			Vector3 ctrlPoint = mesh->GetControlPoint(index);
			ctrlPoint.z += 0.5f;
			mesh->SetControlPoint(index, ctrlPoint);
			break;
		}
		case VK_NEXT:
		{
			// Moves the edge splines middle control point along in -ve Z
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			int index = (mesh->GetControlPointCount() - 1) / 2;
			Vector3 ctrlPoint = mesh->GetControlPoint(index);
			ctrlPoint.z -= 0.5f;
			mesh->SetControlPoint(index, ctrlPoint);
			break;
		}
		// Number 5 Key
		case 0x35: 
		{
			//Cycles through these colours
			std::vector<SimpleMath::Color> colors =
			{
				Colors::Red.v,
				Colors::Green.v,
				Colors::Blue.v
			};

			// Modulo to make sure it never indexes out of the vector
			m_geometries[0]->SetMaterialColor(colors[m_currentColor % colors.size()]);
			m_currentColor++;
			break;
		}
		// Number 6 Key
		case 0x36: 
		{
			// Decreases resolution
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			mesh->SetResolution(mesh->GetResolution() - 1);
			break;
		}
		// Number 7 Key
		case 0x37: 
		{
			// Increases resolution
			NURBSMesh* mesh = dynamic_cast<NURBSMesh*>(m_geometries[0].get());
			mesh->SetResolution(mesh->GetResolution() + 1);
			break;
		}
		default:
		{
			break;
		}
		}
		m_engine.Render(m_geometries);
	}
	case WM_SIZE:
	{
		// Resizes the engine when the shape and size of the view changes
		if (m_engine.IsInitialised() && wParam != SIZE_MINIMIZED)
		{
			try
			{
				m_engine.Resize();
				m_engine.Render(m_geometries);
			}
			catch (const std::exception& ex)
			{
				MessageBox(NULL, ex.what(), "ERROR", MB_OK | MB_ICONERROR);
			}
		}
	}
	case WM_PAINT:
	{
		// Handles redrawing of the rendered image on the background of the window
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}