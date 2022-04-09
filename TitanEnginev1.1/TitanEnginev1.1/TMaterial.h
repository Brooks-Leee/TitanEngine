#pragma once
#include "TShader.h"
#include "TTexTure.h"

struct MaterialConstants
{
	glm::vec4 diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec3 fresnelR0 = { 0.01f, 0.01f, 0.01f };
	float roughness = 0.25f;
};




class TMaterial
{
public:
	TMaterial();
	~TMaterial();

public:
	void addTexture(TTexTure* texture);



public:
	std::string name;	
	int matCBIndex = -1;
	int diffuseSrvHeapIndex = -1;
	int normalMapIndex = -1;
	//int normalSrvHeapIndex;


	std::vector<TTexTure*> textures;
	TShader* shader;


	glm::vec4 diffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
	glm::vec3 fresnelR0 = {0.01f, 0.01f, 0.01f};
	float roughness = 0.25f;

};

