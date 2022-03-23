#pragma once
class TLight
{
public:
	//void setLightColor(glm::vec4 Color)
	//{
	//	LightColor = Color;
	//}

	//void setLightDiretion(glm::vec3 direction)
	//{
	//	LightDirection = direction;
	//}

	//void setIntensity(float intensity)
	//{
	//	Intensity = intensity;
	//}

	//glm::vec4 getLightColor()
	//{
	//	return LightColor;
	//}

	//glm::vec3 getLightDirection()
	//{
	//	return LightDirection;
	//}

	//float getIntensity()
	//{
	//	return Intensity;
	//}

public:
	glm::vec4 LightColor;
	glm::vec3 LightDirection;
	float Intensity;
	
};

