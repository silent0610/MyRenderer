#pragma once
#include  <map>
#include  <functional>
#include "../core/window.h"
#include "../resources/model.h"

/// @brief 保存了渲染单个模型所需的数据, 如果要适应多物体需要一定修改
struct UniformBuffer
{
	Mat4x4f model_matrix;		// 模型变换矩阵
	Mat4x4f view_matrix;		// 观察变换矩阵
	Mat4x4f proj_matrix;		// 投影变换矩阵
	Mat4x4f mvp_matrix;			// MVP变换矩阵
	Mat4x4f normal_matrix;		// 法线变换矩阵

	/// @brief 计算矩阵 ,包括mvp矩阵,
	void CalculateRestMatrix()
	{
		mvp_matrix = proj_matrix * view_matrix * model_matrix;

		// 用于将法线从模型空间变换到世界空间
		// 使用原始变换矩阵的逆转置矩阵
		// 公式推导 https://blog.csdn.net/weixin_44350205/article/details/105804964
		normal_matrix = matrix_invert(model_matrix).Transpose();
	}

	// 光照数据
	Vec3f light_direction;		// 光照方向，从着色点指向光源
	Vec3f light_color;			// 光照颜色
	Vec3f camera_position;		// 相机方向

};


/// @brief 模仿顶点着色器和像素着色器, 顶点着色输出顶点, 经过光栅化插值, 输入像素着色器输出像素
/// 这里这么设计是因为多态, vertexshader是虚函数, 每个shader类的output内容不同,
/// 这样可以统一使用相同的Varings结构体
struct Varyings
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
typedef std::function<Vec4f(int index, Varyings& output)> VertexShader;

// 像素着色器：返回像素的颜色
typedef std::function<Vec4f(Varyings& input)> PixelShader;

/// 学长我想问一下, 我刚写到shader这里, 不是很清楚这里为什么要这么设计一个函数指针, 
/// 为什么要多封装一层, 是可以方便切换Shader吗(不能直接调用虚函数)?
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

	/// @brief 由子类实现,到时候使用Shader的指针指向各个子类,方便shader切换?
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

		// lambda 函数, 调用子类各自实现的顶点着色器和像素着色器
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
	/// @brief 处理事件
	void HandleKeyEvents() override;

	/// @brief 特定属性的枚举类型,需要结合 varying结构体来理解, 为了多态
	enum VaryingAttributes
	{
		VARYING_TEXCOORD = 0,			// 纹理坐标
		VARYING_POSITION_WS = 1,		// 世界空间坐标
		VARYING_NORMAL_WS = 2,			// 世界空间法线
		VARYING_TANGENT_WS = 3			// 世界空间切线
	};

	/// @brief 用于切换颜色模式, 键盘输入
	enum MaterialInspector
	{
		kMaterialInspectorShaded = '1',//键盘的1
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
	/// @brief 不清楚作用
	MaterialInspector material_inspector_;
};

/// @brief PBR光照模型，使用metallic工作流
/// final 表示这个类 不能被继承
class PBRShader final :public Shader
{
public:
	PBRShader(UniformBuffer*uniform_buffer):Shader(uniform_buffer){
		// 非金属的F0值默认为 0.04 , 我记得最低设置为0.04
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
		VARYING_POSITION_WS = 0,		// 世界空间坐标
	};

	CubeMap* skybox_cubemap_;

	/// @brief ???
	std::vector<Vec3f> plane_vertex_ = {
		{0.5f,0.5f,0.5f},			// 右上角
		{-0.5f,0.5f,0.5f},			// 左上角
		{-0.5f,-0.5f,0.5f},		// 左下角
		{0.5f,-0.5f,0.5f} };		// 右下角

	std::vector<int> plane_index_ = { 0,1,2,     0,2,3 };
};