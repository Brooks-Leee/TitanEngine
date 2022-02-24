#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif


#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "MathHelper.h"
#include "StaticMesh.h"

//d3dapp
#include "d3dUtil.h"
#include "GameTimer.h"
#include <crtdbg.h>

#include "d3dApp.h"
#include "UploadBuffer.h"

#include <comdef.h>
#include <fstream>

#include "MathHelper.h"
#include <float.h>
#include <cmath>
#include <WindowsX.h>
