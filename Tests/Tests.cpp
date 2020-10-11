#include "pch.h"
#include "CppUnitTest.h"
#include "Engine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX::SimpleMath;

namespace Tests
{
	HWND CreateHWND()
	{
		return CreateWindowW(
			L"STATIC",
			L"dummy",
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

	TEST_CLASS(EngineTest)
	{
	public:
		
		TEST_METHOD(GivenFreshEngine_WhenGetViewMatrix_ThenIdentityMatrix)
		{
			Engine engine(CreateHWND());
			Matrix viewMatrix = engine.GetViewMatrix();
			Assert::IsTrue(viewMatrix == SimpleMath::Matrix::Identity);
		}
	};
}
