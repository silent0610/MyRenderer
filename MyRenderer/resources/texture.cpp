#include "texture.h"
#include "utility.h"
// lib 
// 这里因为stb_image.h 包含了声明和定义, 但是默认情况下宏 STB_IMAGE_IMPLEMENTATION 未定义, 
// 所以其 实现 在预处理阶段被抛弃,没有被编译, 需要手动编译
#define STB_IMAGE_IMPLEMENTATION	
#include "../lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"
#include <stdexcept>

#pragma region Texture

Texture::Texture(const std::string& file_name)
{
	texture_data_ = stbi_load(file_name.c_str(), &texture_width_, &texture_height_, &texture_channels_, STBI_default);
	has_data_ = (texture_data_ != nullptr);
}
Texture::~Texture()
{
	if (has_data_)stbi_image_free(texture_data_);
}

Vec4f Texture::Sample2D(float u, float v) const
{
	if (!has_data_) return { 1.0f };

	//mark 加载模型时已截断
	////求余数, 同时保证u,v在[0,1]之间,超过1的部分 循环截断
	u = fmod(u, 1);
	v = fmod(v, 1);

	return SampleBilinear(u * texture_width_, v * texture_height_);
}
Vec4f Texture::Sample2D(Vec2f uv) const
{
	if (!has_data_) return { 1.0f };
	//mark 加载模型时已截断
	uv.x = fmod(uv.x, 1);
	uv.y = fmod(uv.y, 1);

	return SampleBilinear(uv.x * texture_width_, uv.y * texture_height_);
};

ColorRGBA Texture::GetPixelColor(int x, int y) const
{
	x = Between(0, texture_width_ - 1, x); //x为列号
	y = Between(0, texture_height_ - 1, y); //y为行号
	ColorRGBA color(1.0); //Vec4f
	if (x >= 0 && x < texture_width_ && y >= 0 && y < texture_height_)
	{
		//使用指针访问像素数据,计算得到 所需像素r的偏移量
		const uint8_t* pixel_offset = texture_data_ + (x + y * texture_width_) * texture_channels_;
		color.r = pixel_offset[0] / 255.0f;
		color.g = pixel_offset[1] / 255.0f;
		color.b = pixel_offset[2] / 255.0f;
		color.a = texture_channels_ >= 4 ? pixel_offset[3] / 255.0f : 1.0f;
	}
	return color;

};

ColorRGBA Texture::SampleBilinear(const float x, const float y)const
{
	const int x1 = static_cast<int>(floor(x));
	const int y1 = static_cast<int>(floor(y));

	const int x2 = static_cast<int>(ceil(x));
	const int y2 = static_cast<int>(ceil(y));
	//     x2,y2
	//x1,y1

	const float t_x = x - x1;
	const float t_y = y - y1;

	//10 11
	//00 01
	const ColorRGBA color00 = GetPixelColor(x1, y1);
	const ColorRGBA color01 = GetPixelColor(x2, y1);
	const ColorRGBA color10 = GetPixelColor(x1, y2);
	const ColorRGBA color11 = GetPixelColor(x2, y2);

	return BilinearInterpolation(color00, color01, color10, color11, t_x, t_y);
}

ColorRGBA Texture::BilinearInterpolation(const ColorRGBA& color00, const ColorRGBA& color01, const ColorRGBA& color10, const ColorRGBA& color11, const float t_x, const float t_y)
{
	const ColorRGBA color0 = vector_lerp(color00, color01, t_x);
	const ColorRGBA color1 = vector_lerp(color10, color11, t_x);

	return vector_lerp(color0, color1, t_y);
}

#pragma endregion

#pragma region Environment Map

CubeMap::CubeMap(const std::string& file_folder, CubeMapType cube_map_type, int mipmap_level)
{
	cube_map_type_ = cube_map_type;
	switch (cube_map_type_)
	{
	case kSkyBox:
		cubemap_[0] = new Texture(file_folder + "m0_px.hdr"); //x 正方向的面 即右侧面
		cubemap_[1] = new Texture(file_folder + "m0_nx.hdr"); //x 负方向的面 即左侧面
		cubemap_[2] = new Texture(file_folder + "m0_py.hdr"); //y 正方向的面 即上方面
		cubemap_[3] = new Texture(file_folder + "m0_ny.hdr"); //y 负方向的面 即下方面
		cubemap_[4] = new Texture(file_folder + "m0_pz.hdr"); //z 正方向的面 即前方面
		cubemap_[5] = new Texture(file_folder + "m0_nz.hdr"); //z 负方向的面 即后方面
		break;
	case kIrradianceMap:
		cubemap_[0] = new Texture(file_folder + "i_px.hdr"); // 顺序与上面一致
		cubemap_[1] = new Texture(file_folder + "i_nx.hdr");
		cubemap_[2] = new Texture(file_folder + "i_py.hdr");
		cubemap_[3] = new Texture(file_folder + "i_ny.hdr");
		cubemap_[4] = new Texture(file_folder + "i_pz.hdr");
		cubemap_[5] = new Texture(file_folder + "i_nz.hdr");
		break;
	case kSpecularMap:
		//根据mipmap_level加载不同的贴图
		cubemap_[0] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_px.hdr"); //顺序与上面一致
		cubemap_[1] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_nx.hdr");
		cubemap_[2] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_py.hdr");
		cubemap_[3] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_ny.hdr");
		cubemap_[4] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_pz.hdr");
		cubemap_[5] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_nz.hdr");
		break;
	default:
		break;
	}
}

CubeMap::~CubeMap()
{
	for (int i = 0; i < 6; i++)
	{
		delete cubemap_[i];
	}
	delete[] cubemap_;
}

//采样立方体贴图
Vec3f CubeMap::Sample(Vec3f& direction)const
{
	//CalculateCubeMapUV返回结构体, auto进行结构化绑定, 可以得到值里面的值,那么类可以这么操作吗
	const auto [face_id, uv] = CalculateCubeMapUV(direction);
	return cubemap_[face_id]->Sample2D(uv.x, uv.y).xyz();
}


/// @brief  计算立方体贴图的uv坐标和要采样的面,auto进行结构化绑定, 可以得到值里面的值,那么类可以这么操作吗
/// @param direction 用于采样的方向
/// @return 返回结构体
CubeMap::CubeMapUV CubeMap::CalculateCubeMapUV(const Vec3f& direction)
{
	//todo
	CubeMap::CubeMapUV cubemap_uv;
	float ma = 0, sc = 0, tc = 0;
	//确定主轴
	const Vec3f direction_abs = vector_abs(direction);
	if (direction_abs.x > direction_abs.y && direction_abs.x > direction_abs.z)
	{//x为主轴
		ma = direction_abs.x; //因为ma为主轴的绝对值
		if (direction.x > 0)
		{
			cubemap_uv.face_id = 0; //x正方向
			sc = -direction.z;
			tc = -direction.y;
		}
		else
		{
			cubemap_uv.face_id = 1;
			sc = +direction.z;
			tc = -direction.y;
		}
	}
	else if (direction_abs.y > direction_abs.z)		// y轴为主轴
	{
		ma = direction_abs.y;
		if (direction.y > 0)					/* positive y */
		{
			cubemap_uv.face_id = 2;
			sc = +direction.x;
			tc = +direction.z;
		}
		else									/* negative y */
		{
			cubemap_uv.face_id = 3;
			sc = +direction.x;
			tc = -direction.z;
		}
	}
	else											// z轴为主轴
	{
		ma = direction_abs.z;
		if (direction.z > 0)					/* positive z */
		{
			cubemap_uv.face_id = 4;
			sc = +direction.x;
			tc = -direction.y;
		}
		else									/* negative z */
		{
			cubemap_uv.face_id = 5;
			sc = -direction.x;
			tc = -direction.y;
		}
	}
	//根据公式计算uv坐标
	cubemap_uv.uv.u = (sc / ma + 1.0f) / 2.0f;
	cubemap_uv.uv.v = (tc / ma + 1.0f) / 2.0f;

	return  cubemap_uv;
}

SpecularCubeMap::SpecularCubeMap(const std::string& file_folder, CubeMap::CubeMapType cube_map_type)
{
	for (size_t i = 0; i < max_mipmap_level_; i++)
	{
		prefilter_maps_[i] = new CubeMap(file_folder, CubeMap::kSpecularMap, i);
	}
}

IBLMap::IBLMap(const std::string& skybox_path)
{
	if (!CheckFileExist(skybox_path))
	{
		std::cout <<"天空盒本体文件不存在, 去下载一个先"<<std::endl;
		exit(0);
	}
	skybox_name_ = GetFileNameWithoutExtension(skybox_path);//获取路径的天空盒的名字, 这里需要额外的天空盒文件
	//folder存放调用cmgen生成的文件
	//在天空盒路径下的以天空盒名字为名的文件夹  天空盒路径/天空盒名字/
	skybox_folder_ = GetFileFolder(skybox_path) + "/" + skybox_name_ + "/"; 
	
	// 检查并生成IBL
	if (!CheckFileExist(skybox_folder_ + "brdf_lut.hdr"))
	{
		GenerateCubeMap(skybox_path);
	}
	// 加载IBL资源
	skybox_cubemap_ = new CubeMap(skybox_folder_, CubeMap::CubeMapType::kSkyBox);
	irradiance_cubemap_ = new CubeMap(skybox_folder_, CubeMap::CubeMapType::kIrradianceMap);
	specular_cubemap_ = new SpecularCubeMap(skybox_folder_, CubeMap::CubeMapType::kSpecularMap);
	brdf_lut_ = new Texture(skybox_folder_ + "brdf_lut.hdr");

}

#pragma endregion