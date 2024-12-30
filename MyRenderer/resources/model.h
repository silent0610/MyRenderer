#pragma once

#include <vector>
#include "math.h"
#include "texture.h"

struct Attribute {
	Vec3f position_os;
	Vec3f normal_os;
	Vec2f texcoord;
	Vec4f tangent_os;
};

class Model {
public:

	//���ļ�����ģ��,ʹ��tiny_loader
	Model(const std::string& model_path, const Mat4x4f& model_matrix);
	//��������ģ��
	Model(const std::vector<Vec3f>& vertex, const std::vector<int>& index);

	std::string PrintModelInfo() const;

	~Model();
	//�����������ͻ�ȡ��Ӧ�� �����������͵��ַ���
	static std::string GetTextureType(TextureType texture_type);
	//���������ļ�·��,����·��,�ļ���,��������,�����ʽ
	static std::string GetTextureFileName(const std::string& file_path, const std::string& file_name, 
		TextureType texture_type, const std::string& texture_format);

	std::vector<Attribute> attributes_;
	Mat4x4f model_matrix_;

	std::string model_name_, model_folder_;

	int vertex_number_, face_number_;

	//����ָ��
	Texture* base_color_map_;
	Texture* normal_map_;
	Texture* roughness_map_;
	Texture* metallic_map_;
	Texture* occlusion_map_;
	Texture* emission_map_;

	bool has_tangent_ = false;



private: 
	void LoadModel(const std::string& model_name);
	void LoadModelByTinyObj(const std::string& model_name); //tiny_loader


};