#pragma once

#include "../resources/texture.h"
#include "../resources/model.h"
#include "shader.h"
#include "../core/window.h"

inline std::vector<std::string> model_paths =
{
	"../assets/helmet/helmet.obj"
	// "../assets/Cerberus/Cerberus.obj",
	// "../assets/Safe/Safe.obj",
	// "../assets/Revolver_bolter/Revolver_bolter.obj"
};

inline std::vector<Mat4x4f> model_matrices =
{
	matrix_set_rotate(1.0f, 0.0f, 0.0f, -kPi * 0.5f) * matrix_set_scale(1, 1, 1),
	matrix_set_translate(-1.0f, 0.0f, 0.0f) * matrix_set_scale(2, 2, 2),
	matrix_set_translate(-0.2f, -1.0f, -0.2f) * matrix_set_scale(0.03f, 0.03f, 0.03f),
	matrix_set_translate(0,0,0.5f) * matrix_set_scale(1, 1, 1)
};