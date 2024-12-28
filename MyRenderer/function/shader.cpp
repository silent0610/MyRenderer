#include "shader.h"

#pragma region ToneMapping
//似乎没用到吗
#pragma endregion


#pragma Region BlingPhong

/// @brief 这个似乎可以改进, 把position_cs也放在 Varyings结构体中,更统一
/// @param index 
/// @param output 
/// @return 
Vec4f BlinnPhongShader::VertexShaderFunction(int index, Varyings& output)const
{
	//mvp矩阵得到裁剪空间坐标,这时还没有进行透视除法
	Vec4f position_cs = uniform_buffer_->mvp_matrix * attributes_[index].position_os.xyz1();
	const Vec3f positionWS = (uniform_buffer_->model_matrix * attributes_[index].position_os.xyz1()).xyz();
	const Vec3f normalWS = (uniform_buffer_->normal_matrix * attributes_[index].normal_os.xyz1()).xyz();
	const Vec4f tangentWS = uniform_buffer_->model_matrix * attributes_[index].tangent_os;

	output.varying_vec2f[VARYING_TEXCOORD] = attributes_[index].texcoord;
	output.varying_vec3f[VARYING_POSITION_WS] = positionWS;
	output.varying_vec3f[VARYING_NORMAL_WS] = normalWS;
	output.varying_vec4f[VARYING_TANGENT_WS] = tangentWS;
	
	return position_cs;
}

Vec4f BlinnPhongShader::PixelShaderFunction( Varyings& input) const{
	// 准备数据
	Vec2f uv = input.varying_vec2f[VARYING_TEXCOORD];

	Vec3f normal_ws = input.varying_vec3f[VARYING_NORMAL_WS];
	//如果有法线贴图且有数据
	if(model_->normal_map_->has_data_){
		Vec4f tangent_ws = input.varying_vec4f[VARYING_TANGENT_WS];
		Vec3f perturbNormal = (model_->normal_map_->Sample2D(uv)).xyz();
		perturbNormal = perturbNormal * 2.0f - Vec3f(1.0f);// 从[0,1]映射回[-1,1]
		normal_ws = calculate_normal(normal_ws, tangent_ws, perturbNormal);//使用TBN矩阵计算法线
	}
	//由于精度和插值问题,需要重新归一化法线
	normal_ws = NormalizeVector(normal_ws);

	Vec3f positionWS = input.varying_vec3f[VARYING_POSITION_WS];
	Vec3f lightColor = uniform_buffer_->light_color;
	Vec3f lightDir = NormalizeVector(-uniform_buffer_->light_direction);
	Vec3f viewDir = NormalizeVector(uniform_buffer_->camera_position - positionWS);

	//漫反射
	Vec3f baseColor = model_->base_color_map_->Sample2D(uv).xyz();
	Vec3f diffuse = lightColor * baseColor * Saturate(vector_dot(lightDir, normal_ws));

	//高光
	Vec3f halfDir = NormalizeVector(viewDir + lightDir);//半向量
	float specularIntensity = pow(Saturate(vector_dot(normal_ws, halfDir)), 64);//64?是这样最好吗
	Vec3f specular = lightColor * specularIntensity;

	//环境光
	Vec3f ambientColor = baseColor * Vec3f(0.1f);

	Vec3f shadedColor = ambientColor + diffuse + specular;

	Vec3f displayColor;
	//根据模式选择对应的颜色
	switch (material_inspector_)
	{
	case kMaterialInspectorShaded:			displayColor = shadedColor;	break;
	case kMaterialInspectorBaseColor:		displayColor = baseColor;		break;
	case kMaterialInspectorNormal:			displayColor = normal_ws;		break;
	case kMaterialInspectorWorldPosition:	displayColor = positionWS;	break;
	case kMaterialInspectorAmbient:			displayColor = ambientColor;	break;
	case kMaterialInspectorDiffuse:			displayColor = diffuse;		break;
	case kMaterialInspectorSpecular:		displayColor = specular;		break;
	default:								displayColor = shadedColor;
	}
	return displayColor.xyz1();
}


void BlinnPhongShader::HandleKeyEvents()
{
	for (MaterialInspector i = kMaterialInspectorShaded;
		i <= kMaterialInspectorSpecular;
		i = static_cast<MaterialInspector>(i + 1))
	{
		if (window_->keys_[i])
		{
			material_inspector_ = i;
			if (i == kMaterialInspectorShaded)
			{
				window_->RemoveLogMessage("Material Inspector");
			}
			else
			{
				window_->SetLogMessage("Material Inspector", "Material Inspector: " + material_inspector_name_[i - 49]);
			}
			return;
		}
	}
}
#pragma endregion


#pragma region PBR实现 Todo


#pragma endregion

#pragma region SkyBox
// mark 天空盒在这里是具有真实mesh的吗

Vec4f SkyBoxShader::VertexShaderFunction(int index, Varyings& output) const
{
	Vec4f position_cs = uniform_buffer_->mvp_matrix * attributes_[index].position_os.xyz1();
	const Vec3f position_ws = (uniform_buffer_->model_matrix * attributes_[index].position_os.xyz1()).xyz();

	output.varying_vec3f[VARYING_POSITION_WS] = position_ws;
	return position_cs;
}
Vec4f SkyBoxShader::PixelShaderFunction(Varyings& input) const
{
	Vec3f position_ws = input.varying_vec3f[VARYING_POSITION_WS];		// 世界空间坐标
	return  skybox_cubemap_->Sample(position_ws).xyz1();
}
#pragma endregion 