#pragma once

#include "../resources/texture.h"
#include "../resources/model.h"
#include "shader.h"
#include "../core/window.h"




inline std::vector<Mat4x4f> model_matrices =
{
	matrix_set_translate(0.0f, 0.0f, 0.0f)* matrix_set_scale(1, 1, 1),
	matrix_set_translate(5.0f, 0.0f, 0.0f) * matrix_set_scale(0.3f, 0.3f, 0.3f),
	matrix_set_translate(5.5f, 0.0f, 0.0f) * matrix_set_scale(0.1f, 0.1f, 0.1f),
	//matrix_set_translate(0,0,0.5f) * matrix_set_scale(1, 1, 1)
};
inline std::vector<std::string> skybox_paths =
{
	"../assets/kloofendal_48d_partly_cloudy_puresky/kloofendal_48d_partly_cloudy_puresky.hdr",
	"../assets/spruit_sunrise/spruit_sunrise.hdr",
	"../assets/brown_photostudio/brown_photostudio.hdr",
	"../assets/autumn_forest_04/autumn_forest_04.hdr"
};

class Scene
{
public:
	Scene(std::vector<std::string> model_paths);
	~Scene();

	/// @brief 负责shader的切换
	/// @param pbr_shader 
	/// @param blinn_phong_shader 
	void HandleKeyEvents( BlinnPhongShader* blinn_phong_shader, PBRShader* pbr_shader = nullptr );

	void UpdateShaderInfo(Shader* shader) const;

	void LoadNextModel();
	void LoadPrevModel();

	void LoadNextIBLMap();
	void LoadPrevIBLMap();
public:
	std::vector< Model* >models_;
	Model* current_model_;
	int total_model_count_;
	int current_model_index_;

	std::vector< IBLMap* >iblmaps_;
	IBLMap* current_iblmap_;
	int total_iblmap_count_;
	int current_iblmap_index_;

	Window* window_;
	ShaderType current_shader_type_;

};

