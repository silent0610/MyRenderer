#include "texture.h"

#pragma region Texture

Texture::Texture(const std::string& file_name){
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

	//������, ͬʱ��֤u,v��[0,1]֮��,����1�Ĳ��� ѭ���ض�
	u = fmod(u, 1); 
	v = fmod(v, 1);

	return SampleBilinear(u * texture_width_, v * texture_height_);
}
Vec4f Texture::Sample2D(Vec2f uv) const {
	if (!has_data_) return { 1.0f };
	uv.x = fmod(uv.x, 1);
	uv.y = fmod(uv.y, 1);

	return SampleBilinear(uv.x * texture_width_, uv.y * texture_height_);
};

ColorRGBA Texture::GetPixelColor(int x, int y) const {
	x = Between(0, texture_width_ - 1, x); //xΪ�к�
	y = Between(0, texture_height_ - 1, y); //yΪ�к�
	ColorRGBA color(1.0); //Vec4f
	if (x >= 0 && x < texture_width_ && y >= 0 && y < texture_height_) {
		//ʹ��ָ�������������,����õ� ��������r��ƫ����
		const uint8_t* pixel_offset = texture_data_ + (x + y * texture_width_) * texture_channels_;
		color.r = pixel_offset[0] / 255.0f;
		color.g = pixel_offset[1] / 255.0f;
		color.b = pixel_offset[2] / 255.0f;
		color.a = texture_channels_ >= 4 ? pixel_offset[3] / 255.0f : 1.0f;
	}
	return color;

};

ColorRGBA Texture::SampleBilinear(const float x, const float y)const {
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

ColorRGBA Texture::BilinearInterpolation(const ColorRGBA& color00, const ColorRGBA& color01, const ColorRGBA& color10, const ColorRGBA& color11, const float t_x, const float t_y) {
	const ColorRGBA color0 = vector_lerp(color00, color01, t_x);
	const ColorRGBA color1 = vector_lerp(color10, color11, t_x);

	return vector_lerp(color0, color1, t_y);
}

#pragma endregion

#pragma region Environment Map

CubeMap::CubeMap(const std::string& file_folder, CubeMapType cube_map_type, int mipmap_level) {
	cube_map_type_ = cube_map_type;
	switch (cube_map_type_) {
	case kSkyBox:
		cubemap_[0] = new Texture(file_folder + "m0_px.hdr"); //x ��������� ���Ҳ���
		cubemap_[1] = new Texture(file_folder + "m0_nx.hdr"); //x ��������� �������
		cubemap_[2] = new Texture(file_folder + "m0_py.hdr"); //y ��������� ���Ϸ���
		cubemap_[3] = new Texture(file_folder + "m0_ny.hdr"); //y ��������� ���·���
		cubemap_[4] = new Texture(file_folder + "m0_pz.hdr"); //z ��������� ��ǰ����
		cubemap_[5] = new Texture(file_folder + "m0_nz.hdr"); //z ��������� ������
		break;
	case kIrradianceMap:
		cubemap_[0] = new Texture(file_folder + "i_px.hdr"); // ˳��������һ��
		cubemap_[1] = new Texture(file_folder + "i_nx.hdr");
		cubemap_[2] = new Texture(file_folder + "i_py.hdr");
		cubemap_[3] = new Texture(file_folder + "i_ny.hdr");
		cubemap_[4] = new Texture(file_folder + "i_pz.hdr");
		cubemap_[5] = new Texture(file_folder + "i_nz.hdr");
		break;
	case kSpecularMap:
		//����mipmap_level���ز�ͬ����ͼ
		cubemap_[0] = new Texture(file_folder + "m" + std::to_string(mipmap_level) + "_px.hdr"); //˳��������һ��
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

CubeMap::~CubeMap() {
	for (int i = 0; i < 6; i++) {
		delete cubemap_[i];
	}
	delete[] cubemap_;
}

//������������ͼ
Vec3f CubeMap::Sample(Vec3f& direction)const {
	//CalculateCubeMapUV���ؽṹ��, auto���нṹ����, ���Եõ�ֵ�����ֵ,��ô�������ô������
	const auto [face_id, uv] = CalculateCubeMapUV(direction); 
	return cubemap_[face_id]->Sample2D(uv.x, uv.y).xyz();
}

// ������������ͼ��uv�����Ҫ��������
//CalculateCubeMapUV���ؽṹ��, auto���нṹ����, ���Եõ�ֵ�����ֵ,��ô�������ô������
CubeMap::CubeMapUV& CubeMap::CalculateCubeMapUV(const Vec3f& direction)
{
	//todo
	;
}

SpecularCubeMap::SpecularCubeMap(const std::string& file_folder, CubeMap::CubeMapType cube_map_type)
{
	for (size_t i = 0; i < max_mipmap_level_; i++)
	{
		prefilter_maps_[i] = new CubeMap(file_folder, CubeMap::kSpecularMap, i);
	}
}

IBLMap::IBLMap(const std::string& skybbox_path) {
	//todo
	;
}

#pragma endregion