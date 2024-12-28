#pragma once
#include  <map>
#include  <functional>
#include "../core/window.h"
#include "../resources/model.h"

struct UniformBuffer
{

	//todo
};

/// @brief ģ�¶�����ɫ����������ɫ��, ������ɫ�������, ������դ����ֵ, ����������ɫ���������
struct Varings
{
	std::map<int, float> varying_float;    // ������ varying �б�
	std::map<int, Vec2f> varying_vec2f;    // ��άʸ�� varying �б�
	std::map<int, Vec3f> varying_vec3f;    // ��άʸ�� varying �б�
	std::map<int, Vec4f> varying_vec4f;    // ��άʸ�� varying �б�
};

/// @brief ��ɫ������
enum ShaderType
{
	kBlinnPhongShader,
	kPbrShader,
	kSkyBoxShader
};


/// @brief ������ɫ�������ض���Ĳü��ռ����� 
/// VertexShader ��һ�����ͣ�����һ��������������������һ�� Vec4f ���͵ĺ�����
typedef std::function<Vec4f(int index, Varings& output)> VertexShader;

// ������ɫ�����������ص���ɫ
typedef std::function<Vec4f(Varings& input)> PixelShader;

//��ɫģ��
class Shader
{
public:
	UniformBuffer* uniform_buffer_;
	Attribute* attributes_;
	Model* model_;

	VertexShader vertex_shader_;
	PixelShader pixel_shader_;

	Window* window_;				// ���ڻ�ȡ�����¼���չʾ�������?

	/// @brief ����ʵ��
	/// @param index 
	/// @param output 
	/// @return 
	virtual  Vec4f VertexShaderFunction(int index, Varings& output) const = 0;
	virtual  Vec4f PixelShaderFunction(Varings& input) const = 0;
	virtual void HandleKeyEvents() = 0;

	~Shader() = default;
	Shader(UniformBuffer* uniform_buffer)
	{
		uniform_buffer_ = uniform_buffer;
		attributes_ = new Attribute[3];
		window_ = Window::GetInstance();

		vertex_shader_ = [&](const int index, Varings& output)->Vec4f
			{
				return VertexShaderFunction(index, output);
			};
		pixel_shader_ = [&](Varings& input)->Vec4f
			{
				return PixelShaderFunction(input);
			};
	}
};

class BlinnPhongShader : public Shader
{
public:
	BlinnPhongShader(UniformBuffer* uniform_buffer) : Shader(uniform_buffer) {};
	Vec4f VertexShaderFunction(int index, Varings& output) const override;
	Vec4f PixelShaderFunction(Varings& input) const override;
	void HandleKeyEvents() override;

	enum VaryingAttributes
	{
		VARYING_TEXCOORD = 0,			// ��������
		VARYING_POSITION_WS = 1,		// ����ռ�����
		VARYING_NORMAL_WS = 2,			// ����ռ䷨��
		VARYING_TANGENT_WS = 3			// ����ռ�����
	};

	enum MaterialInspector
	{
		kMaterialInspectorShaded = '1',
		kMaterialInspectorBaseColor,
		kMaterialInspectorNormal,
		kMaterialInspectorWorldPosition,
		kMaterialInspectorAmbient,
		kMaterialInspectorDiffuse,
		kMaterialInspectorSpecular
	};

	const std::string material_inspector_name_[7] =
	{
		"Shaded",
		"BaseColor",
		"Normal",
		"WorldPosition",
		"Ambient",
		"Diffuse",
		"Specular"
	};
	MaterialInspector material_inspector_;
};