#include <iostream>
#include <fstream>
#include <set>

#include "function/renderer.h"
#include "core/window.h"
#include "resources/model.h"
#include "function/camera.h"
#include "function/scene.h"

void HandleModelSkyboxSwitchEvents(Window* window, Scene* scene, Renderer* mo_renderer);
int main()
{
	constexpr int width = 800;
	constexpr int height = 600;

	Window* window = Window::GetInstance();
	window->WindowInit(width, height, "SoftRenderer");

#pragma region 外部资源加载
	const auto scene = new Scene();

	auto model = scene->current_model_;
	window->SetLogMessage("model_message", model->PrintModelInfo());
	window->SetLogMessage("model_name", "model name: " + scene->current_model_->model_name_);
	window->SetLogMessage("skybox_name", "skybox name: " + scene->current_iblmap_->skybox_name_);
#pragma endregion

}
