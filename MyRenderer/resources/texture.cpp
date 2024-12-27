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
