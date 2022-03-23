#pragma once


struct MaterialConstants
{
	glm::vec4 diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 fresnelR0 = { 0.01f, 0.01f, 0.01f };
	float roughness = 0.25f;
};




class TMaterial
{
public:
	std::string name;	
	int matCBIndex;
	int diffuseSrvHeapIndex;
	//int normalSrvHeapIndex;

	glm::vec4 diffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
	glm::vec3 fresnelR0 = {0.01f, 0.01f, 0.01f};
	float roughness = 0.25f;

};

