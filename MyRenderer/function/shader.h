#pragma once
#include  <map>
#include  <functional>
#include "../core/window.h"
#include "../resources/model.h"

struct UniformBuffer
{

	//todo
};

/// @brief 模仿顶点着色器和像素着色器, 顶点着色输出顶点, 经过光栅化插值, 输入像素着色器输出像素
struct Varings
{
	std::map<int, float> varying_float;    // 浮点数 varying 列表
	std::map<int, Vec2f> varying_vec2f;    // 二维矢量 varying 列表
	std::map<int, Vec3f> varying_vec3f;    // 三维矢量 varying 列表
	std::map<int, Vec4f> varying_vec4f;    // 四维矢量 varying 列表
};

/// @brief 着色器类型
enum ShaderType
{
	kBlinnPhongShader,
	kPbrShader,
	kSkyBoxShader
};


/// @brief 顶点着色器：返回顶点的裁剪空间坐标 
/// VertexShader 是一个类型，代表一个接受两个参数并返回一个 Vec4f 类型的函数。
typedef std::function<Vec4f(int index, Varings& output)> VertexShader;

// 像素着色器：返回像素的颜色
typedef std::function<Vec4f(Varings& input)> PixelShader;

//着色模型
class Shader
{
public:
	UniformBuffer* uniform_buffer_;
	Attribute* attributes_;
	Model* model_;

	VertexShader vertex_shader_;
	PixelShader pixel_shader_;

	Window* window_;				// 用于获取按键事件，展示材质面板?

	/// @brief 子类实现
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
		VARYING_TEXCOORD = 0,			// 纹理坐标
		VARYING_POSITION_WS = 1,		// 世界空间坐标
		VARYING_NORMAL_WS = 2,			// 世界空间法线
		VARYING_TANGENT_WS = 3			// 世界空间切线
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