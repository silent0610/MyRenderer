#include "scene.h"
#include "../resources/utility.h"



Scene::Scene(std::vector<std::string> model_paths)
{
	// 加载模型
	for (size_t i = 0; i < model_paths.size(); i++)
	{
		auto model = new Model(model_paths[i], model_matrices[i]);
		Motion * motion = new Motion(model_matrices[i], window_);
		model->Motion = motion;
		models_.push_back(model);
	}


	// 加载IBL资源
	for (size_t i = 0; i < skybox_paths.size(); i++)
	{
		auto ibl_map = new IBLMap(skybox_paths[i]);
		iblmaps_.push_back(ibl_map);
	}

	total_model_count_ = models_.size();
	current_model_index_ = 0;
	current_model_ = models_[current_model_index_];

	total_iblmap_count_ = iblmaps_.size();
	current_iblmap_index_ = 0;
	current_iblmap_ = iblmaps_[current_iblmap_index_];

	current_shader_type_ = kBlinnPhongShader;
	window_ = Window::GetInstance();
	window_->SetLogMessage("Shading Model", "Shading Model: Blinn Phong");
}
void Scene::HandleKeyEvents(std::vector<Shader*> blinn_phong_shaders, std::vector < Shader*> pbr_shaders)
{
	if (window_->keys_['P'])
	{
		window_->SetLogMessage("Shading Model", "Shading Model: PBR + IBL");
		current_shader_type_ = kPbrShader;

		for(auto pbr_shader: pbr_shaders)
		{
			auto pbr_shader1 = dynamic_cast<PBRShader*>(pbr_shader);
			pbr_shader1->material_inspector_ = PBRShader::kMaterialInspectorShaded;
		}
	
		window_->RemoveLogMessage("Material Inspector");
	}
	else if (window_->keys_['B'])
	{
		window_->SetLogMessage("Shading Model", "Shading Model: Blinn Phong");
		current_shader_type_ = kBlinnPhongShader;
		for (auto blinn_phong_shader : blinn_phong_shaders)
		{
			auto blinn_phong_shader1 = dynamic_cast<BlinnPhongShader*>(blinn_phong_shader);
			blinn_phong_shader1->material_inspector_ = BlinnPhongShader::kMaterialInspectorShaded;
		}
		window_->RemoveLogMessage("Material Inspector");
	}
}

void Scene::UpdateShaderInfo(std::vector<Shader*> shaders) const
{
	// 检查是否是 BlinnPhongShader 类型
	if (const auto blinn_phong_shader = dynamic_cast<BlinnPhongShader*>(shaders[0]))
	{
		for(auto blinn_phong_shader: shaders)
			blinn_phong_shader->model_ = current_model_;
		return;
	}

	if (const auto pbr_shader = dynamic_cast<PBRShader*>(shaders[0]))
	{
		for(int i =0;i<shaders.size();i++){
			auto pbr_shader = dynamic_cast<PBRShader*>(shaders[i]);
			pbr_shader->model_ = current_model_;

			pbr_shader->irradiance_cubemap_ = current_iblmap_->irradiance_cubemap_;
			pbr_shader->specular_cubemap_ = current_iblmap_->specular_cubemap_;
			pbr_shader->brdf_lut_ = current_iblmap_->brdf_lut_;
		}

	}
	if (const auto skybox_shader = dynamic_cast<SkyBoxShader*>(shaders[0]))
	{
		skybox_shader->skybox_cubemap_ = current_iblmap_->skybox_cubemap_;
	}
}

void Scene::LoadNextModel()
{
	current_model_index_ = (current_model_index_ + 1) % total_model_count_;
	current_model_ = models_[current_model_index_];
}

void Scene::LoadPrevModel()
{
	current_model_index_ = (current_model_index_ - 1 + total_model_count_) % total_model_count_;
	current_model_ = models_[current_model_index_];
}

void Scene::LoadNextIBLMap()
{
	current_iblmap_index_ = (current_iblmap_index_ + 1) % total_iblmap_count_;
	current_iblmap_ = iblmaps_[current_iblmap_index_];
}

void Scene::LoadPrevIBLMap()
{
	current_iblmap_index_ = (current_iblmap_index_ - 1 + total_iblmap_count_) % total_iblmap_count_;
	current_iblmap_ = iblmaps_[current_iblmap_index_];
}


