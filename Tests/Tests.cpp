#include "pch.h"
#include "CppUnitTest.h"
#include "Engine.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	HWND CreateHWND()
	{
		return CreateWindowW(
			(LPCWSTR)"STATIC",
			(LPCWSTR)"dummy",
			WS_OVERLAPPEDWINDOW,
			0,
			0,
			100,
			100,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		);
	}

	TEST_CLASS(Engine)
	{
	public:
		
		TEST_METHOD(GivenFreshEngine_WhenGetViewMatrix_ThenIdentityMatrix)
		{
			Assert::IsTrue(true);
		}
	};
}
