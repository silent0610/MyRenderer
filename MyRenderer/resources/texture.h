#pragma once
#include "../core/math.h"
#include <string>


// ��ͼ�������
// ����������ͼ����������ͼ��Ԥ���˵Ļ�����ͼ��IBL��ͼ�ȵ�

enum TextureType
{
	kTextureTypeBaseColor, // ������ɫ��ͼ
	kTextureTypeNormal, // ������ͼ
	kTextureTypeRoughness, // �ֲڶ���ͼ
	kTextureTypeMetallic, // ��������ͼ
	kTextureTypeOcclusion, // �ڵ���ͼ
	kTextureTypeEmission // �Է�����ͼ
};

class Texture
{
public:
	Texture(const std::string& file_name);
	~Texture();

	// ��������
	Vec4f Sample2D(float u, float v)const;
	Vec4f Sample2D(Vec2f uv) const;
	
	int texture_width_;					// ������
	int texture_height_;				// ����߶�
	int texture_channels_;				// ����ͨ����

	bool has_data_;						// �Ƿ�������ݣ����Ƿ�ɹ�������ͼ
	unsigned char* texture_data_;		// ʵ�ʵ�ͼ������
private:
	ColorRGBA GetPixelColor(int x, int y) const;
	ColorRGBA SampleBilinear(float x, float y) const;// ˫���Բ���,ʹ��˫���Բ�ֵ����
	
	// ˫���Բ�ֵ
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
		kSkyBox, // ��պ�
		kIrradianceMap, // irradiance
		kSpecularMap, // ���淴��
	};

	CubeMap(const std::string& file_name, CubeMapType cube_map_type, int mipmap_level=0);
	~CubeMap();

	Vec3f Sample(Vec3f& direction)const;
	static CubeMapUV& CalculateCubeMapUV(const Vec3f& direction);

	Texture* cubemap_[6]; // 6�������ͼ,ָ������
	CubeMapType cube_map_type_;

};

// Ԥ���˵Ļ�����ͼ �߹�?
class SpecularCubeMap
{
public:
	SpecularCubeMap(const std::string& file_folder, CubeMap::CubeMapType cube_map_type);

	static constexpr int max_mipmap_level_ = 10;
	
	CubeMap* prefilter_maps_[max_mipmap_level_];//����ָ��

};

// ��IBL ʵ����պУ���պл����skybox��������ͼ��irradiance��������ͼ����specular��������ͼ
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

