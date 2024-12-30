#pragma once
#include  <map>
#include  <functional>
#include "../core/window.h"
#include "../resources/model.h"

/// @brief ��������Ⱦ����ģ�����������, ���Ҫ��Ӧ��������Ҫһ���޸�
struct UniformBuffer
{
	Mat4x4f model_matrix;		// ģ�ͱ任����
	Mat4x4f view_matrix;		// �۲�任����
	Mat4x4f proj_matrix;		// ͶӰ�任����
	Mat4x4f mvp_matrix;			// MVP�任����
	Mat4x4f normal_matrix;		// ���߱任����

	/// @brief ������� ,����mvp����,
	void CalculateRestMatrix()
	{
		mvp_matrix = proj_matrix * view_matrix * model_matrix;

		// ���ڽ����ߴ�ģ�Ϳռ�任������ռ�
		// ʹ��ԭʼ�任�������ת�þ���
		// ��ʽ�Ƶ� https://blog.csdn.net/weixin_44350205/article/details/105804964
		normal_matrix = matrix_invert(model_matrix).Transpose();
	}

	// ��������
	Vec3f light_direction;		// ���շ��򣬴���ɫ��ָ���Դ
	Vec3f light_color;			// ������ɫ
	Vec3f camera_position;		// �������

};


/// @brief ģ�¶�����ɫ����������ɫ��, ������ɫ�������, ������դ����ֵ, ����������ɫ���������
/// ������ô�������Ϊ��̬, vertexshader���麯��, ÿ��shader���output���ݲ�ͬ,
/// ��������ͳһʹ����ͬ��Varings�ṹ��
struct Varyings
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
typedef std::function<Vec4f(int index, Varyings& output)> VertexShader;

// ������ɫ�����������ص���ɫ
typedef std::function<Vec4f(Varyings& input)> PixelShader;

/// ѧ��������һ��, �Ҹ�д��shader����, ���Ǻ��������ΪʲôҪ��ô���һ������ָ��, 
/// ΪʲôҪ���װһ��, �ǿ��Է����л�Shader��(����ֱ�ӵ����麯��)?
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

	/// @brief ������ʵ��,��ʱ��ʹ��Shader��ָ��ָ���������,����shader�л�?
	/// @param index 
	/// @param output 
	/// @return 
	virtual  Vec4f VertexShaderFunction(int index, Varyings& output) const = 0;
	virtual  Vec4f PixelShaderFunction(Varyings& input) const = 0;
	virtual void HandleKeyEvents() = 0;

	~Shader() = default;
	Shader(UniformBuffer* uniform_buffer)
	{
		uniform_buffer_ = uniform_buffer;
		attributes_ = new Attribute[3];
		window_ = Window::GetInstance();

		// lambda ����, �����������ʵ�ֵĶ�����ɫ����������ɫ��
		vertex_shader_ = [&](const int index, Varyings& output)->Vec4f
			{
				return VertexShaderFunction(index, output);
			};
		pixel_shader_ = [&](Varyings& input)->Vec4f
			{
				return PixelShaderFunction(input);
			};
	}
};

class BlinnPhongShader : public Shader
{
public:
	BlinnPhongShader(UniformBuffer* uniform_buffer) : Shader(uniform_buffer) {};
	Vec4f VertexShaderFunction(int index, Varyings& output) const override;
	Vec4f PixelShaderFunction(Varyings& input) const override;
	/// @brief �����¼�
	void HandleKeyEvents() override;

	/// @brief �ض����Ե�ö������,��Ҫ��� varying�ṹ�������, Ϊ�˶�̬
	enum VaryingAttributes
	{
		VARYING_TEXCOORD = 0,			// ��������
		VARYING_POSITION_WS = 1,		// ����ռ�����
		VARYING_NORMAL_WS = 2,			// ����ռ䷨��
		VARYING_TANGENT_WS = 3			// ����ռ�����
	};

	/// @brief �����л���ɫģʽ, ��������
	enum MaterialInspector
	{
		kMaterialInspectorShaded = '1',//���̵�1
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
	/// @brief ���������
	MaterialInspector material_inspector_;
};

/// @brief PBR����ģ�ͣ�ʹ��metallic������
/// final ��ʾ����� ���ܱ��̳�
class PBRShader final :public Shader
{
public:
	PBRShader(UniformBuffer*uniform_buffer):Shader(uniform_buffer){
		// �ǽ�����F0ֵĬ��Ϊ 0.04 , �Ҽǵ��������Ϊ0.04
		dielectric_f0_ = Vec3f(0.04f);
		material_inspector_ = kMaterialInspectorShaded;

		//
		use_lut_ = false;
	}
	Vec4f VertexShaderFunction(int index, Varyings& output)const override;
	Vec4f PixelShaderFunction(Varyings& input) const override;
	void HandleKeyEvents()override;

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
		kMaterialInspectorRoughness,
		kMaterialInspectorMetallic,
		kMaterialInspectorOcclusion,
		kMaterialInspectorEmission
	};

	const std::string material_inspector_name_[8] =
	{
		"Shaded",
		"BaseColor",
		"Normal",
		"WorldPosition",
		"Roughness",
		"Metallic",
		"Occlusion",
		"Emission"
	};
	MaterialInspector material_inspector_;
	Vec3f dielectric_f0_;

	CubeMap* irradiance_cubemap_;
	SpecularCubeMap* specular_cubemap_;
	Texture* brdf_lut_;

	bool use_lut_;
};

class SkyBoxShader final :public Shader
{
public:
	SkyBoxShader(UniformBuffer* uniformBuffer):Shader(uniformBuffer){}
	Vec4f VertexShaderFunction(int index, Varyings& output) const override;
	Vec4f PixelShaderFunction(Varyings& input) const override;
	void HandleKeyEvents() override {};

	enum VaryingAttributes
	{
		VARYING_POSITION_WS = 0,		// ����ռ�����
	};

	CubeMap* skybox_cubemap_;

	/// @brief ???
	std::vector<Vec3f> plane_vertex_ = {
		{0.5f,0.5f,0.5f},			// ���Ͻ�
		{-0.5f,0.5f,0.5f},			// ���Ͻ�
		{-0.5f,-0.5f,0.5f},		// ���½�
		{0.5f,-0.5f,0.5f} };		// ���½�

	std::vector<int> plane_index_ = { 0,1,2,     0,2,3 };
};