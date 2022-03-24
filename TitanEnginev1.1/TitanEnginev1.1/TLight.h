#pragma once
#include "MathHelper.h"


class TLight
{
public:
	glm::vec4 LightColor;
	glm::vec3 LightDirection;
	float Intensity;

	//DirectX::XMFLOAT4 LightColor;
	//DirectX::XMFLOAT3 LightDirection;
	//float Intensity;
};

