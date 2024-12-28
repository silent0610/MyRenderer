#pragma once
#include "../core/math.h"
#include <string>


// 贴图类的声明
// 单张纹理贴图，立方体贴图，预过滤的环境贴图，IBL贴图等等

enum TextureType
{
	kTextureTypeBaseColor, // 基础颜色贴图
	kTextureTypeNormal, // 法线贴图
	kTextureTypeRoughness, // 粗糙度贴图
	kTextureTypeMetallic, // 金属度贴图
	kTextureTypeOcclusion, // 遮挡贴图
	kTextureTypeEmission // 自发光贴图
};

class Texture
{
public:
	Texture(const std::string& file_name);
	~Texture();

	// 采样函数
	Vec4f Sample2D(float u, float v)const;
	Vec4f Sample2D(Vec2f uv) const;
	
	int texture_width_;					// 纹理宽度
	int texture_height_;				// 纹理高度
	int texture_channels_;				// 纹理通道数

	bool has_data_;						// 是否存在数据，即是否成功加载贴图
	unsigned char* texture_data_;		// 实际的图像数据
private:
	ColorRGBA GetPixelColor(int x, int y) const;
	ColorRGBA SampleBilinear(float x, float y) const;// 双线性采样,使用双线性插值函数
	
	// 双线性插值
	static ColorRGBA BilinearInterpolation(const ColorRGBA& color00, const ColorRGBA& color01, const ColorRGBA& color10, const ColorRGBA& color11, float t_x, float t_y);

};

class CubeMap {
public:
	struct CubeMapUV
	{
		int face_id;
		Vec2f uv;
	};

	enum CubeMapType {
		kSkyBox, // 天空盒
		kIrradianceMap, // irradiance
		kSpecularMap, // 镜面反射
	};

	CubeMap(const std::string& file_name, CubeMapType cube_map_type, int mipmap_level=0);
	~CubeMap();

	Vec3f Sample(Vec3f& direction)const;
	static CubeMapUV& CalculateCubeMapUV(const Vec3f& direction);

	Texture* cubemap_[6]; // 6个面的贴图,指针数组
	CubeMapType cube_map_type_;

};

// 预过滤的环境贴图 高光?
class SpecularCubeMap
{
public:
	SpecularCubeMap(const std::string& file_folder, CubeMap::CubeMapType cube_map_type);

	static constexpr int max_mipmap_level_ = 10;
	
	CubeMap* prefilter_maps_[max_mipmap_level_];//数组指针

};

// 用IBL 实现天空盒，天空盒会包括skybox立方体贴图，irradiance立方体贴图，和specular立方体贴图
class IBLMap
{

public:
	IBLMap() = default;
	IBLMap(const std::string& skybox_path);

	CubeMap* skybox_cubemap_;
	CubeMap* irradiance_cubemap_;
	SpecularCubeMap* specular_cubemap_;
	Texture* brdf_lut_;

	std::string skybox_name_;
	std::string skybox_folder_;
};

