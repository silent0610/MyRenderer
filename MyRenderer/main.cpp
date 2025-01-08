#include <iostream>
#include <fstream>
#include <set>

#include "function/renderer.h"
#include "core/window.h"
#include "resources/model.h"
#include "function/camera.h"
#include "function/scene.h"
#include "function/light.h"
#include <thread>
#include <cstdint>
#include <mutex>
#include <chrono>

#include "../core/BS_thread_pool.hpp"

int64_t GetNowTime(){
	static int64_t startTime = -1;
	auto now = std::chrono::system_clock::now();
	auto curTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	
	if (startTime == -1)
	{
		startTime = curTime;
		return 0;
	}
	return curTime - startTime;
}

uint8_t* color_buffer;
float** DepthBuffer;
constexpr int width = 800;
constexpr int height = 600;
Vec4f color_background = Vec4f(0.5f, 1.0f, 1.0f, 1.0f);
Vec4f color_foreground = Vec4f(0.0f);
std::mutex mtx;
void HandleModelSkyboxSwitchEvents(Window* window, Scene* scene, std::vector<Renderer*> renderers);
inline std::vector<std::string> model_paths =
{
	// "../assets/helmet/helmet.obj"
	"../assets/sun/sun.obj",
	"../assets/earth/earth.obj",
	"../assets/moon/moon.obj"

	 //"../assets/Cerberus/Cerberus.obj"
	// "../assets/Safe/Safe.obj",
	// "../assets/Revolver_bolter/Revolver_bolter.obj"
};

void ClearFrameBuffer(bool clear_color_buffer, bool clear_depth_buffer)
{

	if (clear_color_buffer && color_buffer)
	{
		const ColorRGBA32Bit color_32_bit = vector_to_32bit_color(color_background);

		for (int j = 0; j < height; j++)
		{
			const int offset = width * (4 * j);
			for (int i = 0; i < width; i++)
			{
				const int base_address = offset + 4 * i;
				//32 bit位图存储顺序，从低到高依次为BGRA
				color_buffer[base_address] = color_32_bit.b;
				color_buffer[base_address + 1] = color_32_bit.g;
				color_buffer[base_address + 2] = color_32_bit.r;
				color_buffer[base_address + 3] = color_32_bit.a;
			}
		}
	}

	if (clear_depth_buffer && DepthBuffer)
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
				DepthBuffer[j][i] = 0.0f;
		}
	}
}


void ProcessThread(int start, int num, Model* model,Shader* shader, Renderer* renderer, UniformBuffer* uniform_buffer, Scene* scene)
{
	// auto time = GetNowTime();

	int i = 0, j = 0, k = 0;
	int trigianleNum = model->face_number_;
	int end = ((start+num) < trigianleNum)? (start + num): trigianleNum;
	for (i = start; i < end; i++)
	{
		j = i * 3;
		for (k = 0; k < 3; k++)
		{
			shader->attributes_[k].position_os = model->attributes_[j + k].position_os;
			shader->attributes_[k].texcoord = model->attributes_[j + k].texcoord;
			shader->attributes_[k].normal_os = model->attributes_[j + k].normal_os;
			shader->attributes_[k].tangent_os = model->attributes_[j + k].tangent_os;
		}
		renderer->DrawMesh();
	};
	// std::cout << "thread " << i <<" " << GetNowTime() - time<<std::endl;
	
	return;
}
void MultiMultiThreadedRender()
{

	BS::thread_pool pool;
	int numThreads = pool.get_thread_count();


	Window* window = Window::GetInstance();
	window->WindowInit(width, height, "SoftRenderer");

#pragma region 外部资源加载
	auto scene = new Scene(model_paths);
	auto model = scene->current_model_;

	window->SetLogMessage("model_message", model->PrintModelInfo());
	window->SetLogMessage("model_name", "model name: " + scene->current_model_->model_name_);
	window->SetLogMessage("skybox_name", "skybox name: " + scene->current_iblmap_->skybox_name_);
#pragma endregion

#pragma region 配置UniformBuffer, 相机参数, 光源参数
	const Vec3f camera_position = { 0, 0, 5 };	// 相机位置
	const Vec3f camera_target = { 0, 0, 0 };	// 相机看向的位置
	const Vec3f camera_up = { 0, 1, 0 };		// 相机向上的位置
	constexpr float fov = 90.0f;				// 相加的垂直FOV
	auto* camera = new Camera(camera_position, camera_target, camera_up, fov, static_cast<float>(width) / height);
	auto light = PointLight(Vec3f(1.0f), Vec3f(0.0f));
	// auto light = DirectionalLight(Vec3f(1.0f),Vec3f(0.0f,-5.0f,-2.0f));

	const auto uniform_buffer = new UniformBuffer();

	uniform_buffer->view_matrix = matrix_look_at(camera_position, camera_target, camera_up);
	uniform_buffer->proj_matrix = matrix_set_perspective(fov, camera->aspect_, camera->near_plane_, camera->near_plane_);

	uniform_buffer->light = &light;

	uniform_buffer->camera_position = camera->position_;
#pragma endregion

	// 初始化帧缓冲
	color_buffer = new uint8_t[width * height * 4];
	DepthBuffer = new float* [width];
	for (int j = 0; j < height; j++)
	{
		DepthBuffer[j] = new float[width];
	}

#pragma region 配置Renderer

	const auto skybox_shader = new SkyBoxShader(uniform_buffer);

	// 初始化渲染器
	std::vector<Shader*> blinn_phong_shaders;
	std::vector<Shader*> pbr_shaders;
	std::vector<Shader*> skybox_shaders;
	skybox_shaders.push_back(skybox_shader);
	std::vector<Renderer*> renderers;
	
	for (int i = 0; i < numThreads; ++i)
	{
		auto blinn_phong_shader = new BlinnPhongShader(uniform_buffer);
		auto pbr_shader = new PBRShader(uniform_buffer);
		blinn_phong_shaders.push_back(blinn_phong_shader);
		pbr_shaders.push_back(pbr_shader);
		auto renderer = new Renderer(width, height, color_buffer, DepthBuffer);
		renderer->SetRenderState(false, true);
		renderers.push_back(renderer);
	}
#pragma endregion
#pragma region 旋转初始化
	CircleMovement* orbitMovement = new CircleMovement(1.0f, 1.0f);
	float time = window->PlatformGetTime();
	float deltaTime = 0.0f;
	float curTime = 0.0f;
#pragma endregion
#pragma region rendererloop
	int loop = 16;
	while (!window->is_close_)
	{
		//std::cout << "frame start time: ";

		HandleModelSkyboxSwitchEvents(window, scene, renderers);	//修改为改变所有renderer的状态	// 切换天空盒和模型，切换线框渲染
		camera->HandleInputEvents();									// 更新相机参数
		scene->HandleKeyEvents(blinn_phong_shaders, pbr_shaders);			// 更新当前使用的shader

		switch (scene->current_shader_type_)
		{
		case kBlinnPhongShader:
			scene->UpdateShaderInfo(blinn_phong_shaders);
			for(int i=0;i<renderers.size();++i){
				renderers[i]->SetVertexShader(blinn_phong_shaders[i]->vertex_shader_);
				renderers[i]->SetPixelShader(blinn_phong_shaders[i]->pixel_shader_);
			}
			for (auto blinn_phong_shader: blinn_phong_shaders)
				blinn_phong_shader->HandleKeyEvents();
			break;
		case kPbrShader:
			scene->UpdateShaderInfo(pbr_shaders);
			for (int i = 0; i < renderers.size(); ++i)
			{
				renderers[i]->SetVertexShader(pbr_shaders[i]->vertex_shader_);
				renderers[i]->SetPixelShader(pbr_shaders[i]->pixel_shader_);
			}
			for (auto pbr_shader : pbr_shaders)
			{
				pbr_shader->HandleKeyEvents();
			}
			break;
		default:
			break;
		}

#pragma region 逻辑 旋转
		//公转
		curTime = window->PlatformGetTime();
		deltaTime = curTime - time;
		time = curTime;
		Mat4x4f orbitMatrix = orbitMovement->GetMovementMatrix(deltaTime);
		for (auto model : scene->models_)
		{
			model->Motion->modelMatrix = orbitMatrix * model->Motion->modelMatrix;
		}
		//月球绕地球转
		Vec3f axis = scene->models_[1]->Motion->modelMatrix.GetCol(3).xyz();
		Mat4x4f moveToOrigin = matrix_set_translate(-axis.x, -axis.y, -axis.z);
		Mat4x4f moveBack = matrix_set_translate(axis.x, axis.y, axis.z);
		Mat4x4f rotate = matrix_set_rotate(0, 1, 0, 0.2f);
		scene->models_[2]->Motion->modelMatrix = moveBack * rotate * moveToOrigin * scene->models_[2]->Motion->modelMatrix;
#pragma endregion
		ClearFrameBuffer(renderers[0]->render_frame_, true);
		//std::cout << "multi model start time: ";
		for (auto model : scene->models_)
		{
			switch (scene->current_shader_type_)
			{
			case kBlinnPhongShader:
				camera->UpdateUniformBuffer(blinn_phong_shaders[0]->uniform_buffer_, model->Motion->modelMatrix);
				for (auto blinn_phong_shader: blinn_phong_shaders)
					blinn_phong_shader->model_ = model;
				break;
			case kPbrShader:
				camera->UpdateUniformBuffer(pbr_shaders[0]->uniform_buffer_,model->Motion->modelMatrix);
				for (auto pbr_shader : pbr_shaders)
					pbr_shader->model_ = model;
				break;
			default:
				break;
			}
			//  划分三角形
			std::vector<std::vector<int>> trigianlesPerThread;
			int trigianleCount = model->face_number_;
			int tragianleNumPerThread = std::ceil(trigianleCount / numThreads);

			//// 单线程处理
			//ProcessThread(0, trigianleCount, model, blinn_phong_shaders[0], renderers[0], uniform_buffer, scene);
			
			for (unsigned int i = 0; i < numThreads; ++i)
			{	
				int start = i * tragianleNumPerThread;
				if (start >= model->face_number_) 
					continue;
				Shader* blp_shader = blinn_phong_shaders[i];
				Shader* pbr_shader = pbr_shaders[i];
				Renderer* renderer = renderers[i];
				switch(scene->current_shader_type_){
					case kBlinnPhongShader:
						pool.detach_task([start, tragianleNumPerThread, model, blp_shader, renderer, uniform_buffer, scene] { ProcessThread(start, tragianleNumPerThread, model, blp_shader, renderer, uniform_buffer, scene); });
						break;
					case kPbrShader:
						pool.detach_task([start, tragianleNumPerThread, model, pbr_shader, renderer, uniform_buffer, scene] { ProcessThread(start, tragianleNumPerThread, model, pbr_shader, renderer, uniform_buffer, scene);});
						break;
					default:
						break;
				}
			}
			pool.wait();

		}

#pragma region 渲染Skybox
		scene->UpdateShaderInfo(skybox_shaders);
		renderers[0]->SetVertexShader(skybox_shader->vertex_shader_);
		renderers[0]->SetPixelShader(skybox_shader->pixel_shader_);
		renderers[0]->depth_buffer_ = DepthBuffer;
		camera->UpdateSkyBoxUniformBuffer(skybox_shader->uniform_buffer_);
		camera->HandleInputEvents();
		camera->UpdateSkyboxMesh(skybox_shader);
		//window->WindowDisplay(color_buffer);
		for (size_t i = 0; i < skybox_shader->plane_vertex_.size() - 2; i++)
		{
			skybox_shader->attributes_[0].position_os = skybox_shader->plane_vertex_[0];
			skybox_shader->attributes_[1].position_os = skybox_shader->plane_vertex_[i + 1];
			skybox_shader->attributes_[2].position_os = skybox_shader->plane_vertex_[i + 2];

			renderers[0]->DrawSkybox();
		}

#pragma endregion

		window->WindowDisplay(color_buffer);
		
	}
#pragma endregion

}



int main(){
	std::cout << "program start time: " << GetNowTime() << std::endl;
	MultiMultiThreadedRender();
	return 0;
}

void HandleModelSkyboxSwitchEvents(Window* window, Scene* scene, std::vector<Renderer*> renderers)
{
	if (window->can_press_keyboard_)
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			scene->LoadPrevModel();
			window->SetLogMessage("model_message", scene->current_model_->PrintModelInfo());
			window->SetLogMessage("model_name", "model name: " + scene->current_model_->model_name_);
			window->can_press_keyboard_ = false;

		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			scene->LoadNextModel();
			window->SetLogMessage("model_message", scene->current_model_->PrintModelInfo());
			window->SetLogMessage("model_name", "model name: " + scene->current_model_->model_name_);
			window->can_press_keyboard_ = false;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			scene->LoadPrevIBLMap();
			window->SetLogMessage("skybox_name", "skybox name: " + scene->current_iblmap_->skybox_name_);
			window->can_press_keyboard_ = false;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			scene->LoadNextIBLMap();
			window->SetLogMessage("skybox_name", "skybox name: " + scene->current_iblmap_->skybox_name_);
			window->can_press_keyboard_ = false;
		}
		else if (window->keys_['0'])					// 切换渲染模式：线框渲染-像素渲染
		{
			if (renderers[0]->render_frame_)
			{
				for (auto renderer : renderers)
					renderer->SetRenderState(false, true);
			}
			else
			{
				for (auto renderer : renderers)
					renderer->SetRenderState(true, false);
			}
		}
		window->can_press_keyboard_ = false;
	}

}





