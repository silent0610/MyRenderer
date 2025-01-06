#include "light.h"
Light::Light(Vec3f lightColor) : lightColor(lightColor)
{
}

PointLight::PointLight(Vec3f lightColor, Vec3f position) : Light(lightColor), lightPosition(position)
{
}
Vec3f PointLight::GetLightDir(Vec3f shadePosition){
	return lightPosition - shadePosition;
};

DirectionalLight::DirectionalLight(Vec3f lightColor, Vec3f direction) : Light(lightColor), lightDirection(direction)
{
}

Vec3f DirectionalLight::GetLightDir(Vec3f shadePosition)
{
	return lightDirection;
};