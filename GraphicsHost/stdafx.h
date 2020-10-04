// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"


// reference additional headers your program requires here

using namespace Microsoft::WRL;