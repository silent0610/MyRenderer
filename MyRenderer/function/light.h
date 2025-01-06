#pragma once
#include <../core/math.h>
class Light
{
public:
	Light(Vec3f lightColor);
	~Light() = default;
	virtual Vec3f GetLightDir(Vec3f shadePosition = Vec3f(0.0f)) = 0;
	Vec3f lightColor;
}; 

class PointLight : public Light
{
public:
	/// @brief 
	/// @param lightColor 光源颜色
	/// @param position 
	PointLight(Vec3f lightColor, Vec3f position);
	~PointLight() = default;
	Vec3f GetLightDir(Vec3f shadePosition = Vec3f(0.0f));
	Vec3f lightPosition;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Vec3f lightColor, Vec3f direction);
	~DirectionalLight() = default;
	Vec3f GetLightDir(Vec3f shadePosition = Vec3f(0.0f));
	/// @brief 光源发出光
	Vec3f lightDirection;
};