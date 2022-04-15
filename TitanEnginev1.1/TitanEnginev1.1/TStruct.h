#pragma once
class TStruct
{
};

struct TViewPort
{
	float TopLeftX = 0;
	float TopLeftY = 0;
	float Width = 800.0f;
	float Height = 600.0f;
	float MinDepth = 0.0f;
	float MaxDepth = 1.0f;

	int ClientWidth = 800;
	int ClientHeight = 600;
};

enum PRIMITIVE_TOPOLOGY
{
	PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
	PRIMITIVE_TOPOLOGY_POINTLIST = 1,
	PRIMITIVE_TOPOLOGY_LINELIST = 2,
	PRIMITIVE_TOPOLOGY_LINESTRIP = 3,	
	PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
	PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 10,
	PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 11,
	PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 12, 
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 13
};

enum RENDERBUFFER_TYPE
{
	COMMAND_RENDER_BUFFER = 0,
	DEPTHSTENCIL_BUFFER = 1,
	HDR_RENDER_BUFFER = 2
};

enum TEX_FORMAT
{
	FORMAT_R8G8B8A8_UNORM = 0,
	FORMAT_D24_UNORM_S8_UINT = 1,
	FORMAT_R16G16B16A16_FLOAT = 2,
	FORMAT_UNKNOWN = 3
};