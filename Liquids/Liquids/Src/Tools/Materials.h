#pragma once
#include "glm/glm.hpp"
struct Material
{
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float opacity = 1.0f;
};

static Material PolishedBronze = {
	glm::vec3(0.4f,	0.2368f	,0.1036f),
	glm::vec3(0.774597f,	0.458561f,	0.200621f),
	76.8f,
	1.0f
};
static Material Gold = {
glm::vec3(0.75164f,	0.60648f,	0.22648f),
glm::vec3(0.628281f,	0.555802f,	0.366065f),	
51.2f,
1.0f
};
static Material Glass = {
glm::vec3(0.588235f, 0.670588f, 0.729412f),
glm::vec3(0.9f,0.9f,0.9f),
25.6f,
0.2f
};
static Material BlueGlass =
{
	glm::vec3(0.0f, 0.0f, 0.729412f),
	glm::vec3(0.9f,0.9f,0.9f),
	25.6f,
	0.2f
};
static Material RedGlass =
{
	glm::vec3(0.729412f, 0.0f, 0.0f),
	glm::vec3(0.9f,0.9f,0.9f),
	25.6f,
	0.3f
};