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

Vec4f BlinnPhongShader::PixelShaderFunction(Varyings& input) const
{
	// 准备数据
	Vec2f uv = input.varying_vec2f[VARYING_TEXCOORD];

	Vec3f normal_ws = input.varying_vec3f[VARYING_NORMAL_WS];
	//如果有法线贴图且有数据
	if (model_->normal_map_->has_data_)
	{
		Vec4f tangent_ws = input.varying_vec4f[VARYING_TANGENT_WS];
		Vec3f perturbNormal = (model_->normal_map_->Sample2D(uv)).xyz();
		perturbNormal = perturbNormal * 2.0f - Vec3f(1.0f);// 从[0,1]映射回[-1,1]
		normal_ws = calculate_normal(normal_ws, tangent_ws, perturbNormal);//使用TBN矩阵计算法线
	}
	//由于精度和插值问题,需要重新归一化法线
	normal_ws = NormalizeVector(normal_ws);

	Vec3f positionWS = input.varying_vec3f[VARYING_POSITION_WS];
	Vec3f lightColor = uniform_buffer_->light->lightColor;
	Vec3f lightDir = NormalizeVector(-uniform_buffer_->light->GetLightDir(positionWS));
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

	//自发光, 仅用于计算机图形学大作业
	if (model_->emission_map_->has_data_)
	{
		Vec3f emission = model_->emission_map_->Sample2D(uv).xyz();
		diffuse += emission;
	}


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


#pragma region PBR实现 
// 菲涅尔反射率的Schlick近似值，详见RTR4 章节9.5 ,公式9.16
// 在F0和F90（白色）之间进行插值，使用pow函数进行拟合，在接近掠射角度下快速增长
// 由于镜面微表面的假设，因此half_dir就是微表面的法线，见章节 9.8 
// 所以这里有一个镜面微表面的假设, 就是每个微表面都是镜面反射的
Vec3f FresnelSchlickApproximation(const Vec3f& m, const Vec3f& light_dir, const Vec3f& f0)
{
	const float m_dot_l = Saturate(vector_dot(m, light_dir));
	return f0 + (Vec3f(1.0f) - f0) * pow(1.0f - m_dot_l, 5.0f);
}
// GGX法线分布函数，详见RTR4章节9.8中的方程9.41
float D_GGX_Original(const Vec3f& m, const Vec3f& n, const float roughness)
{
	const float n_dot_m = Saturate(vector_dot(n, m));
	const float n_dot_m_2 = n_dot_m * n_dot_m;

	const float roughness2 = roughness * roughness;

	const float factor = 1.0f + n_dot_m_2 * (roughness2 - 1.0f);
	const float D = n_dot_m * roughness2 / (kPi * factor * factor);

	return D;
}
// Smith G1函数，详见RTR4章节9.7中的方程9.24
// 使用GGX分布的lambda函数，详见章节9.8中的方程9.37和方程9.42
// 参数中的s可能是view_dir或者light_dir
float Smith_G1_GGX(const Vec3f& m, const Vec3f& n, const Vec3f& s, const float roughness)
{
	const float m_dot_s = Saturate(vector_dot(m, s));
	const float n_dot_s = Saturate(vector_dot(n, s));
	const float n_dot_s_2 = n_dot_s * n_dot_s;

	const float roughness2 = roughness * roughness;

	const float a2_reciprocal = roughness2 * (1 - n_dot_s_2) / (n_dot_s_2 + kEpsilon);
	const float lambda = (sqrtf(1.0f + a2_reciprocal) - 1.0f) * 0.5f;

	const float Smith_G1 = m_dot_s / (1.0f + lambda);
	return Smith_G1;
}

// 最简单形式的G2函数，详见RTR4章节9.7中的方程9.27
float Smith_G2_GGX(const Vec3f& m, const Vec3f& n,
	const Vec3f& light_dir, const Vec3f& view_dir,
	const float roughness)
{
	const float g1_shadowing = Smith_G1_GGX(m, n, light_dir, roughness);
	const float g1_masking = Smith_G1_GGX(m, n, view_dir, roughness);

	return  g1_shadowing * g1_masking;
}


static float ACESToneMapping(float value)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	value = (value * (a * value + b)) / (value * (c * value + d) + e);
	return Between(0.0f, 1.0f, value);
}

static float GammaCorrection(float value)
{
	return  pow(value, 1.0f / 2.2f);
}

static Vec3f& PostProcessing(Vec3f& color)
{
	for (int i = 0; i < 3; i++)
	{
		color[i] = ACESToneMapping(color[i]);
	}
	return color;
}

Vec4f PBRShader::VertexShaderFunction(int index, Varyings& output) const
{
	Vec4f position_cs = uniform_buffer_->mvp_matrix * attributes_[index].position_os.xyz1();
	const Vec3f position_ws = (uniform_buffer_->model_matrix * attributes_[index].position_os.xyz1()).xyz();
	const Vec3f normal_ws = (uniform_buffer_->normal_matrix * attributes_[index].normal_os.xyz1()).xyz();
	if (model_->has_tangent_)
	{
		const Vec4f tangent_ws = uniform_buffer_->model_matrix * attributes_[index].tangent_os;
		output.varying_vec4f[VARYING_TANGENT_WS] = tangent_ws;
	}

	output.varying_vec2f[VARYING_TEXCOORD] = attributes_[index].texcoord;
	output.varying_vec3f[VARYING_POSITION_WS] = position_ws;
	output.varying_vec3f[VARYING_NORMAL_WS] = normal_ws;
	return position_cs;
}

Vec4f PBRShader::PixelShaderFunction(Varyings& input)const
{
	Vec2f uv = input.varying_vec2f[VARYING_TEXCOORD];
	Vec3f position_ws = input.varying_vec3f[VARYING_POSITION_WS];
	Vec3f normal_ws = input.varying_vec3f[VARYING_NORMAL_WS]; // 由顶点法线导出的法线
	if (model_->normal_map_->has_data_ && model_->has_tangent_)
	{
		Vec4f tangent_ws = input.varying_vec4f[VARYING_TANGENT_WS]; //模型自带的切线
		Vec3f perturbNormal = (model_->normal_map_->Sample2D(uv)).xyz(); // 模型的法线贴图
		perturbNormal = perturbNormal * 2.0f - Vec3f(1.0f);
		normal_ws = calculate_normal(normal_ws, tangent_ws, perturbNormal);  // 使用TBN矩阵计算法线
	}
	normal_ws = NormalizeVector(normal_ws);
	// 阴影, 多线程,

	float metallic = model_->metallic_map_->Sample2D(uv).b;
	float perceptual_roughness = model_->roughness_map_->Sample2D(uv).b;
	float roughness = perceptual_roughness * perceptual_roughness; //从感知粗糙度到粗糙度

	Vec3f occlusion(1.0f);
	if (model_->occlusion_map_->has_data_)
	{
		occlusion = model_->occlusion_map_->Sample2D(uv).b;
	}
	Vec3f emission(0.0f);
	if (model_->emission_map_->has_data_)
	{
		emission = model_->emission_map_->Sample2D(uv).b;
	}
	Vec3f baseColor{ 0.9f, 0.0f, 0.5f };
	if (model_->base_color_map_->has_data_)
	{
		baseColor = model_->base_color_map_->Sample2D(uv).xyz();
	}
	Vec3f lightColor = uniform_buffer_->light_color;
	Vec3f lightDir = NormalizeVector(-uniform_buffer_->light_direction);
	Vec3f viewDir = NormalizeVector(uniform_buffer_->camera_position - position_ws);
	Vec3f halfDir = NormalizeVector(viewDir + lightDir);

	float n_dot_l = vector_dot(normal_ws, lightDir);
	float n_dot_l_abs = Abs(n_dot_l);
	n_dot_l = Saturate(n_dot_l);

	float n_dot_v = vector_dot(normal_ws, viewDir);
	float n_dot_v_abs = Abs(n_dot_v);
	n_dot_v = Saturate(n_dot_v);

	// 直接光照
	
	// 高光
	Vec3f f0 = vector_lerp(dielectric_f0_, baseColor, metallic);					// 获取材质的F0值, 公式9.16
	Vec3f F = FresnelSchlickApproximation(halfDir, lightDir, f0);					// 菲涅尔, 见RTR4 chapter9 

	float D = D_GGX_Original(halfDir, normal_ws, roughness); 					// GGX法线分布函数

	float G = Smith_G2_GGX(halfDir, normal_ws, lightDir, viewDir, roughness); // 几何遮挡函数 ,shaodwing-masking function

	Vec3f cook_torrance_brdf = (D * G) * F / (4.0f * n_dot_l_abs * n_dot_v_abs + kEpsilon); // 高光BRDF 见公式9.34

	// 漫反射
	// lambertian BRDF 是完美漫反射, 认为所有光线都被均匀地反射, 如果考虑能量守恒和高光项, 即表面反射的光线并不能用于次表面散射
	// 所以要减去 菲涅尔系数, 见公式9.62
	Vec3f kd = (Vec3f(1.0) - F) * (1 - metallic);  // (1-F)指射向表面的光被反射的部分, (1-metallic)是次表面反射的部分
	Vec3f lambertian_brdf = baseColor; // 漫反射BRDF, 这里不除pi, 因为 积分计算会得到一个pi, 两者抵消. 可以化简一个除法, pi 是哪来的详见公式9.10

	Vec3f radiance_direct = (kd * lambertian_brdf + cook_torrance_brdf) * lightColor * n_dot_l;  //直接光照颜色计算

	// IBL image based lighting

	// specular ibl
	int max_mipmap_level = SpecularCubeMap::max_mipmap_level_ - 1;
	int specular_mipmap_level = roughness * max_mipmap_level + 0.5f; //图10.37上

	Vec3f reflected_view_dir = vector_reflect(viewDir, normal_ws);
	Vec3f prefilter_specular_color = specular_cubemap_->prefilter_maps_[specular_mipmap_level]->Sample(reflected_view_dir);
	
	Vec2f lut_uv = { n_dot_v ,roughness };
	Vec2f lut_sample = brdf_lut_->Sample2D(lut_uv).xy();

	float specular_scale = lut_sample.x;
	float specular_bias = lut_sample.y;
	Vec3f specular = f0 * specular_scale + Vec3f(specular_bias);
	Vec3f radiance_specular_ibl = prefilter_specular_color * specular;

	// diffuse ibl
	Vec3f irradiance = irradiance_cubemap_->Sample(normal_ws);
	Vec3f radiance_diffuse_ibl = kd * irradiance * baseColor;

	Vec3f radiance_ibl = (radiance_diffuse_ibl + radiance_specular_ibl) * occlusion;

	// 最终颜色
	Vec3f shaded_color = (radiance_direct + radiance_ibl) + emission;

	Vec3f display_color;
	switch (material_inspector_)
	{
	case kMaterialInspectorShaded:			display_color = PostProcessing(shaded_color);	break;
	case kMaterialInspectorBaseColor:		display_color = baseColor;		break;
	case kMaterialInspectorNormal:			display_color = normal_ws;		break;
	case kMaterialInspectorWorldPosition:	display_color = position_ws;	break;
	case kMaterialInspectorRoughness:		display_color = roughness;	break;
	case kMaterialInspectorMetallic:		display_color = metallic;		break;
	case kMaterialInspectorOcclusion:		display_color = occlusion;	break;
	case kMaterialInspectorEmission:		display_color = emission;		break;

	default:								display_color = shaded_color;
	}
	return display_color.xyz1();
}

void PBRShader::HandleKeyEvents()
{
	for (MaterialInspector i = kMaterialInspectorShaded;
		i <= kMaterialInspectorEmission;
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

#pragma region SkyBox
// 天空盒在这里是具有真实mesh的吗 是的

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