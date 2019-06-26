#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <unordered_map>
struct Light{
	glm::vec4 LightSource;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float Constant;
	float Linear;
	float Quadratic;
	Light()
	{
		Constant = 0; Linear = 0; Quadratic = 0;
	}
	void setSource(float f1, float f2, float f3, float f4)
	{
		LightSource = glm::vec4(f1, f2, f3, f4);
	}
	void setAmbient(float f1, float f2, float f3)
	{
		ambient = glm::vec3(f1, f2, f3);
	}
	glm::vec3 GetDiffuse() { return diffuse; }
	void setDiffuse(float f1, float f2, float f3)
	{
		diffuse = glm::vec3(f1, f2, f3);
	}
	void setSpecular(float f1, float f2, float f3)
	{
		specular = glm::vec3(f1, f2, f3);
	}
	void setConstant(float f)
	{
		Constant = f;
	}
	void setLinear(float f)
	{
		Linear = f;
	}
	void setQuadratic(float f)
	{
		Quadratic = f;
	}
	void setAttenuation(float f1, float f2, float f3)
	{
		setConstant(f1); setLinear(f2); setQuadratic(f3);
	}
};
class LightClass {
private:
	std::unordered_map<std::string, std::pair<Light,bool>> lights;
public:
	void AddLight(Light light, std::string Identifier);
	void DeleteLight(std::string Identifier);
	void EnableLight(std::string Identifier);
	void DisableLight(std::string Identifier);
	void ToggleLight(std::string Identifier);
	void EnableAll();
	int GetActiveLightsNumber() const;
	std::vector<Light> GetActiveLightsVector() const;
	Light* GetLight(std::string Identifier);
};