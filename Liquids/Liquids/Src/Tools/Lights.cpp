#include "Lights.h"
void LightClass::AddLight(Light light, std::string Identifier)
{
	lights[Identifier].second=true;
	lights[Identifier].first = light;
}
void LightClass::DeleteLight(std::string Identifier)
{
	lights.erase(Identifier);
}
void LightClass::EnableLight(std::string Identifier)
{
	lights[Identifier].second=true;
}
void LightClass::EnableAll()
{
	for (auto& element : lights)
		element.second.second = true;
}
void LightClass::ToggleLight(std::string Identifier)
{
	lights[Identifier].second = !lights[Identifier].second;
}
void LightClass::DisableLight(std::string Identifier)
{
	lights[Identifier].second=false;
}
int LightClass::GetActiveLightsNumber() const
{
	int n=0;
	for (auto p : lights)
		if (p.second.second)
			n++;
	return n;
}
std::vector<Light> LightClass::GetActiveLightsVector() const
{
	std::vector<Light> vec;
	for (auto p : lights)
		if (p.second.second)
			vec.push_back(p.second.first);
	return vec;
}
Light* LightClass::GetLight(std::string Identifier)
{
	return &lights[Identifier].first;
}