#pragma once
#include "MathHelper.h"


class TLight
{
public:
	glm::vec4 LightColor;
	glm::vec3 LightDirection;
	float Intensity;

	glm::mat4 lightVP;
	glm::mat4 lightTVP;
};

