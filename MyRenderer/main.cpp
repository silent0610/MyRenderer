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
void PrintTime()
{
	static int64_t startTime = -1;
	auto now = std::chrono::system_clock::now();
	// auto curTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	auto curTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

	if (startTime == -1)
	{
		startTime = curTime;
		std::cout << "start time(us): " << curTime << std::endl;
		return;
	}
	std::cout << "current time(us): " << curTime - startTime << std::endl;
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
	/*"../assets/helmet/helmet.obj",*/
	"../assets/sun/sun.obj",
	"../assets/earth/earth.obj",
	"../assets/moon/moon.obj"

	// "../assets/Cerberus/Cerberus.obj",
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
//struct Trigianle
//{
//	int modelIndex;
//	int triangleIndex;
//};
//std::vector<std::vector<Trigianle>>* SplitTrigianles(std::vector< Model* >* models)
//{
//	unsigned int num_threads = std::thread::hardware_concurrency();
//	int trigianleCount = 0;
//	for (int i = 0; i < models->size(); i++)
//	{
//		trigianleCount += (*models)[i]->face_number_;
//		std::cout << trigianleCount << std::endl;
//	}
//	int tragianleNumPerThread = trigianleCount / num_threads;
//	int count = 0;
//	int index = 0;
//	std::vector<std::vector<Trigianle>>* threadTrigianles = new std::vector<std::vector<Trigianle>>;
//	for (int i = 0; i < 16; i++)
//	{
//		threadTrigianles->push_back(std::vector<Trigianle>());
//	}
//	for (int i = 0; i < models->size(); i++)
//	{
//		Model* curModel = (*models)[i];
//		for (int j = 0; j < curModel->face_number_; j++)
//		{
//			count += 1;
//			Trigianle trigianle{ i,j };
//
//			(*threadTrigianles)[index].push_back(trigianle);
//			//std::cout << i << " " << j << std::endl;
//			if (count == tragianleNumPerThread)
//			{
//				count = 0;
//				index += 1;
//			}
//		}
//	}
//	return threadTrigianles;
//}

////读取模型矩阵, 读取
//void ProcessThread(std::vector<Trigianle>& trigianles, UniformBuffer* unifrombuffer, Scene* scene, Renderer renderer)
//{
//	UniformBuffer* uniformBuffer = unifrombuffer;
//	const auto blinn_phong_shader = new BlinnPhongShader(uniformBuffer);
//	const auto pbr_shader = new PBRShader(uniformBuffer);
//
//	switch (scene->current_shader_type_)
//	{
//	case kBlinnPhongShader:
//		scene->UpdateShaderInfo(blinn_phong_shader);
//		renderer.SetVertexShader(blinn_phong_shader->vertex_shader_);
//		renderer.SetPixelShader(blinn_phong_shader->pixel_shader_);
//
//		blinn_phong_shader->HandleKeyEvents();
//		break;
//	case kPbrShader:
//		scene->UpdateShaderInfo(pbr_shader);
//		renderer.SetVertexShader(pbr_shader->vertex_shader_);
//		renderer.SetPixelShader(pbr_shader->pixel_shader_);
//
//		pbr_shader->HandleKeyEvents();
//		break;
//	default:
//		break;
//	}
//	for (auto& trigianle : trigianles)
//	{
//		switch (scene->current_shader_type_)
//		{
//		case kBlinnPhongShader:
//			blinn_phong_shader->uniform_buffer_->model_matrix = scene->models_[trigianle.modelIndex]->Motion->modelMatrix;
//			uniformBuffer->CalculateRestMatrix();
//			blinn_phong_shader->model_ = scene->models_[trigianle.modelIndex];
//			break;
//		case kPbrShader:
//			pbr_shader->uniform_buffer_->model_matrix = scene->models_[trigianle.modelIndex]->Motion->modelMatrix;
//			uniformBuffer->CalculateRestMatrix();
//			pbr_shader->model_ = scene->models_[trigianle.modelIndex];
//			break;
//		default:
//			break;
//		}
//		Model* curModel = scene->models_[trigianle.modelIndex];
//		int i = trigianle.triangleIndex * 3;
//
//		for (int j = 0; j < 3; j++)
//		{
//
//			switch (scene->current_shader_type_)
//			{
//			case kBlinnPhongShader:
//				blinn_phong_shader->attributes_[j].position_os = curModel->attributes_[i + j].position_os;
//				blinn_phong_shader->attributes_[j].texcoord = curModel->attributes_[i + j].texcoord;
//				blinn_phong_shader->attributes_[j].normal_os = curModel->attributes_[i + j].normal_os;
//				blinn_phong_shader->attributes_[j].tangent_os = curModel->attributes_[i + j].tangent_os;
//				break;
//			case kPbrShader:
//				pbr_shader->attributes_[j].position_os = curModel->attributes_[i + j].position_os;
//				pbr_shader->attributes_[j].texcoord = curModel->attributes_[i + j].texcoord;
//				pbr_shader->attributes_[j].normal_os = curModel->attributes_[i + j].normal_os;
//				pbr_shader->attributes_[j].tangent_os = curModel->attributes_[i + j].tangent_os;
//				break;
//			}
//		}
//		renderer.DrawMesh();
//
//	}
//
//}
void PBRProcessThread(int start, int num, Model* model,Shader *shader, Renderer *renderer, UniformBuffer *uniform_buffer,Scene *scene) {
	int i = 0, j = 0, k = 0;
	for (i = start; i < start + num; i++)
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
}
void BLPProcessThread(int start, int num, Model* model, Shader * shader, Renderer* renderer, UniformBuffer* uniform_buffer, Scene* scene)
{
	int i = 0, j = 0, k = 0;
	for (i = start; i < start + num; i++)
	{
		j = i * 3;
		//for (k = 0; k < 3; k++)
		//{
		//	shader->attributes_[k].position_os = model->attributes_[j + k].position_os;
		//	shader->attributes_[k].texcoord = model->attributes_[j + k].texcoord;
		//	shader->attributes_[k].normal_os = model->attributes_[j + k].normal_os;
		//	shader->attributes_[k].tangent_os = model->attributes_[j + k].tangent_os;
		//}
		//renderer->DrawMesh();
	};
}
void ProcessThread(int start, int num, Model* model,Shader* shader, Renderer* renderer, UniformBuffer* uniform_buffer, Scene* scene)
{
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
	return;
}
void MultiMultiThreadedRender()
{
	int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;

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


	const auto uniform_buffer = new UniformBuffer();
	//uniform_buffer->model_matrix = model->model_matrix_;
	uniform_buffer->view_matrix = matrix_look_at(camera_position, camera_target, camera_up);
	uniform_buffer->proj_matrix = matrix_set_perspective(fov, camera->aspect_, camera->near_plane_, camera->near_plane_);
	//uniform_buffer->CalculateRestMatrix();
	uniform_buffer->light = &light;
	//uniform_buffer->light_direction = { 0, -5, -2 };
	//uniform_buffer->light_color = Vec3f(1.0f);
	uniform_buffer->camera_position = camera->position_;
#pragma endregion

	// 初始化帧缓冲
	color_buffer = new uint8_t[width * height * 4];
	DepthBuffer = new float* [width];
	for (int j = 0; j < height; j++)
	{
		DepthBuffer[j] = new float[width];
	}
	//多线程三角形划分
	//std::vector<std::vector<Trigianle>>* trigianles = SplitTrigianles(&(scene->models_));
	/*std::cout << "thread size: " << trigianles->size() << std::endl;*/
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
	while (!window->is_close_)
	{
		std::cout << "frame start time: ";
		PrintTime();
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
		std::cout << "multi model start time: ";
		PrintTime();
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
				camera->UpdateUniformBuffer(pbr_shaders[0]->uniform_buffer_, model->model_matrix_);
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
			//model->attributes_
			std::cout << "multi thread start time: ";
			PrintTime();
			for (unsigned int i = 0; i < numThreads; ++i)
			{
				int start = i * tragianleNumPerThread;
				if (start >= model->face_number_) 
					continue;
				//ProcessThread(start, tragianleNumPerThread, model, pbr_shaders[i], renderers[i], uniform_buffer, scene);
				switch(scene->current_shader_type_){
					case kBlinnPhongShader:
						threads.push_back(std::thread(ProcessThread, start, tragianleNumPerThread, model, blinn_phong_shaders[i], renderers[i], uniform_buffer, scene));
						break;
					case kPbrShader:
						threads.push_back(std::thread(ProcessThread, start, tragianleNumPerThread, model,pbr_shaders[i], renderers[i], uniform_buffer, scene));
						break;
					default:
						break;
				}
			}
			for (auto& th : threads)
			{
				th.join();
			}
			threads.clear();
			std::cout << "one model multi thread end time: ";
			PrintTime();
		}
		std::cout << "multi model end time: ";
		PrintTime();
		std::cout << "skybox start time: ";
		PrintTime();
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
		std::cout << "skybox end time: ";
		PrintTime();
#pragma endregion

		window->WindowDisplay(color_buffer);
		
	}
#pragma endregion

}



int main(){
	std::cout << "program start time: ";
	PrintTime();
	MultiMultiThreadedRender();
	return 0;
}
//int main()
//{
//
//	constexpr int width = 800;
//	constexpr int height = 600;
//
//	Window* window = Window::GetInstance();
//	window->WindowInit(width, height, "SoftRenderer");
//
//#pragma region 外部资源加载
//	const auto scene = new Scene(model_paths);
//	auto model = scene->current_model_;
//
//	window->SetLogMessage("model_message", model->PrintModelInfo());
//	window->SetLogMessage("model_name", "model name: " + scene->current_model_->model_name_);
//	window->SetLogMessage("skybox_name", "skybox name: " + scene->current_iblmap_->skybox_name_);
//#pragma endregion
//
//	//多线程三角形划分
//	std::vector<std::vector<Trigianle>>* trigianles = SplitTrigianles(&(scene->models_));
//	std::cout << "thread size: " << trigianles->size() << std::endl;
//	//std::cout << "trigianles[0] size: " << (*trigianles)[0].size() << std::endl;
//#pragma region 配置UniformBuffer, 相机参数, 光源参数
//	const Vec3f camera_position = { 0, 0, 5 };	// 相机位置
//	const Vec3f camera_target = { 0, 0, 0 };	// 相机看向的位置
//	const Vec3f camera_up = { 0, 1, 0 };		// 相机向上的位置
//	constexpr float fov = 90.0f;				// 相加的垂直FOV
//	auto* camera = new Camera(camera_position, camera_target, camera_up, fov, static_cast<float>(width) / height);
//	auto light = PointLight(Vec3f(1.0f), Vec3f(0.0f));
//
//
//	const auto uniform_buffer = new UniformBuffer();
//	uniform_buffer->model_matrix = model->model_matrix_;
//	uniform_buffer->view_matrix = matrix_look_at(camera_position, camera_target, camera_up);
//	uniform_buffer->proj_matrix = matrix_set_perspective(fov, camera->aspect_, camera->near_plane_, camera->near_plane_);
//	uniform_buffer->CalculateRestMatrix();
//	uniform_buffer->light = &light;
//	//uniform_buffer->light_direction = { 0, -5, -2 };
//	//uniform_buffer->light_color = Vec3f(1.0f);
//	uniform_buffer->camera_position = camera->position_;
//#pragma endregion
//
//#pragma region 配置Renderer
//	const auto blinn_phong_shader = new BlinnPhongShader(uniform_buffer);
//	const auto pbr_shader = new PBRShader(uniform_buffer);
//	const auto skybox_shader = new SkyBoxShader(uniform_buffer);
//
//	// 初始化渲染器
//	const auto renderer = new Renderer(width, height, color_buffer, depth_buffer);
//	renderer->SetRenderState(false, true);
//#pragma endregion
//
//#pragma region 旋转初始化
//	CircleMovement* orbitMovement = new CircleMovement(1.0f, 1.0f);
//	//scene->models_[0]->Motion->Movements[0] = orbitMovement;
//	//scene->models_[1]->Motion->Movements[0] = orbitMovement;
//	//scene->models_[2]->Motion->Movements[0] = orbitMovement;
//
//	float time = window->PlatformGetTime();
//	float deltaTime = 0.0f;
//	float curTime = 0.0f;
//#pragma endregion
//#pragma region RenderLoop
//	int index = 0;
//	while (!window->is_close_)
//	{
//		HandleModelSkyboxSwitchEvents(window, scene, renderer);		// 切换天空盒和模型，切换线框渲染
//		camera->HandleInputEvents();									// 更新相机参数
//		scene->HandleKeyEvents(blinn_phong_shader, pbr_shader);			// 更新当前使用的shader
//
//#pragma region 渲染Model
//#pragma region 多物体
//		switch (scene->current_shader_type_)
//		{
//		case kBlinnPhongShader:
//			scene->UpdateShaderInfo(blinn_phong_shader);
//			renderer->SetVertexShader(blinn_phong_shader->vertex_shader_);
//			renderer->SetPixelShader(blinn_phong_shader->pixel_shader_);
//
//			blinn_phong_shader->HandleKeyEvents();
//			break;
//		case kPbrShader:
//			scene->UpdateShaderInfo(pbr_shader);
//			renderer->SetVertexShader(pbr_shader->vertex_shader_);
//			renderer->SetPixelShader(pbr_shader->pixel_shader_);
//
//			pbr_shader->HandleKeyEvents();
//			break;
//		default:
//			break;
//		}
//		renderer->ClearFrameBuffer(renderer->render_frame_, true);
//
//#pragma region 日地月旋转
//		//公转
//		curTime = window->PlatformGetTime();
//		deltaTime = curTime - time;
//		time = curTime;
//		Mat4x4f orbitMatrix = orbitMovement->GetMovementMatrix(deltaTime);
//		for (auto model : scene->models_)
//		{
//			model->Motion->modelMatrix = orbitMatrix * model->Motion->modelMatrix;
//		}
//		//月球绕地球转
//		Vec3f axis = scene->models_[1]->Motion->modelMatrix.GetCol(3).xyz();
//		Mat4x4f moveToOrigin = matrix_set_translate(-axis.x, -axis.y, -axis.z);
//		Mat4x4f moveBack = matrix_set_translate(axis.x, axis.y, axis.z);
//		Mat4x4f rotate = matrix_set_rotate(0, 1, 0, 0.2f);
//		scene->models_[2]->Motion->modelMatrix = moveBack * rotate * moveToOrigin * scene->models_[2]->Motion->modelMatrix;
//#pragma endregion
//
//		//更新VP矩阵
//		switch (scene->current_shader_type_)
//		{
//		case kBlinnPhongShader:
//			camera->UpdateUniformBuffer(blinn_phong_shader->uniform_buffer_, model->Motion->modelMatrix);
//			break;
//		case kPbrShader:
//			camera->UpdateUniformBuffer(pbr_shader->uniform_buffer_, model->model_matrix_);
//			break;
//		default:
//			break;
//		}
//
//
//		for (auto& curThreadTrigianles : *trigianles)
//		{
//			//std::cout << curThreadTrigianles.size() << std::endl;
//
//			for (auto& trigianle : curThreadTrigianles)
//			{
//				switch (scene->current_shader_type_)
//				{
//				case kBlinnPhongShader:
//					blinn_phong_shader->uniform_buffer_->model_matrix = scene->models_[trigianle.modelIndex]->Motion->modelMatrix;
//					uniform_buffer->CalculateRestMatrix();
//					blinn_phong_shader->model_ = scene->models_[trigianle.modelIndex];
//					break;
//				case kPbrShader:
//					pbr_shader->uniform_buffer_->model_matrix = scene->models_[trigianle.modelIndex]->Motion->modelMatrix;
//					uniform_buffer->CalculateRestMatrix();
//					pbr_shader->model_ = scene->models_[trigianle.modelIndex];
//					break;
//				default:
//					break;
//				}
//				Model* curModel = scene->models_[trigianle.modelIndex];
//				int i = trigianle.triangleIndex * 3;
//
//				//if (trigianle.modelIndex != index){
//				//	
//				//	std::cout << index << " change to  " << trigianle.modelIndex << std::endl;
//				//	index = trigianle.modelIndex;
//				//	std::cout << "model matrix " << std::endl << curModel->Motion->modelMatrix << std::endl;
//				//}
//
//				for (int j = 0; j < 3; j++)
//				{
//
//					switch (scene->current_shader_type_)
//					{
//					case kBlinnPhongShader:
//						blinn_phong_shader->attributes_[j].position_os = curModel->attributes_[i + j].position_os;
//						blinn_phong_shader->attributes_[j].texcoord = curModel->attributes_[i + j].texcoord;
//						blinn_phong_shader->attributes_[j].normal_os = curModel->attributes_[i + j].normal_os;
//						blinn_phong_shader->attributes_[j].tangent_os = curModel->attributes_[i + j].tangent_os;
//						break;
//					case kPbrShader:
//						pbr_shader->attributes_[j].position_os = curModel->attributes_[i + j].position_os;
//						pbr_shader->attributes_[j].texcoord = curModel->attributes_[i + j].texcoord;
//						pbr_shader->attributes_[j].normal_os = curModel->attributes_[i + j].normal_os;
//						pbr_shader->attributes_[j].tangent_os = curModel->attributes_[i + j].tangent_os;
//						break;
//					}
//				}
//				renderer->DrawMesh();
//			}
//
//		}
//
//
//		//for (auto model : scene->models_)
//		//{
//		//	switch (scene->current_shader_type_)
//		//	{
//		//	case kBlinnPhongShader:
//		//		camera->UpdateUniformBuffer(blinn_phong_shader->uniform_buffer_, model->Motion->modelMatrix);
//		//		blinn_phong_shader->model_ = model;
//		//		break;
//		//	case kPbrShader:
//		//		camera->UpdateUniformBuffer(pbr_shader->uniform_buffer_, model->model_matrix_);
//		//		pbr_shader->model_ = model;
//		//		break;
//		//	default:
//		//		break;
//		//	}
//		//	for (size_t i = 0; i < model->attributes_.size(); i += 3)
//		//	{
//		//		// 设置三个顶点的输入，供 VS 读取
//		//		for (int j = 0; j < 3; j++)
//		//		{
//		//			switch (scene->current_shader_type_)
//		//			{
//		//			case kBlinnPhongShader:
//		//				blinn_phong_shader->attributes_[j].position_os = model->attributes_[i + j].position_os;
//		//				blinn_phong_shader->attributes_[j].texcoord = model->attributes_[i + j].texcoord;
//		//				blinn_phong_shader->attributes_[j].normal_os = model->attributes_[i + j].normal_os;
//		//				blinn_phong_shader->attributes_[j].tangent_os = model->attributes_[i + j].tangent_os;
//		//				break;
//		//			case kPbrShader:
//		//				pbr_shader->attributes_[j].position_os = model->attributes_[i + j].position_os;
//		//				pbr_shader->attributes_[j].texcoord = model->attributes_[i + j].texcoord;
//		//				pbr_shader->attributes_[j].normal_os = model->attributes_[i + j].normal_os;
//		//				pbr_shader->attributes_[j].tangent_os = model->attributes_[i + j].tangent_os;
//		//				break;
//		//			}
//		//		}
//		//		// 绘制三角形
//		//		renderer->DrawMesh();
//		//	}
//
//		//}
//#pragma endregion
//
//	//model = scene->current_model_;
//	//switch (scene->current_shader_type_)
//	//{
//	//case kBlinnPhongShader:
//	//	scene->UpdateShaderInfo(blinn_phong_shader);
//	//	renderer->SetVertexShader(blinn_phong_shader->vertex_shader_);
//	//	renderer->SetPixelShader(blinn_phong_shader->pixel_shader_);
//	//	camera->UpdateUniformBuffer(blinn_phong_shader->uniform_buffer_, model->model_matrix_);
//
//	//	blinn_phong_shader->HandleKeyEvents();
//	//	break;
//	//case kPbrShader:
//	//	scene->UpdateShaderInfo(pbr_shader);
//	//	renderer->SetVertexShader(pbr_shader->vertex_shader_);
//	//	renderer->SetPixelShader(pbr_shader->pixel_shader_);
//	//	camera->UpdateUniformBuffer(pbr_shader->uniform_buffer_, model->model_matrix_);
//
//	//	pbr_shader->HandleKeyEvents();
//	//	break;
//	//default:
//	//	break;
//	//}
//	//renderer->ClearFrameBuffer(renderer->render_frame_, true);
//	//for (size_t i = 0; i < model->attributes_.size(); i += 3)
//	//{
//	//	// 设置三个顶点的输入，供 VS 读取
//	//	for (int j = 0; j < 3; j++)
//	//	{
//	//		switch (scene->current_shader_type_)
//	//		{
//	//		case kBlinnPhongShader:
//	//			blinn_phong_shader->attributes_[j].position_os = model->attributes_[i + j].position_os;
//	//			blinn_phong_shader->attributes_[j].texcoord = model->attributes_[i + j].texcoord;
//	//			blinn_phong_shader->attributes_[j].normal_os = model->attributes_[i + j].normal_os;
//	//			blinn_phong_shader->attributes_[j].tangent_os = model->attributes_[i + j].tangent_os;
//	//			break;
//	//		case kPbrShader:
//	//			pbr_shader->attributes_[j].position_os = model->attributes_[i + j].position_os;
//	//			pbr_shader->attributes_[j].texcoord = model->attributes_[i + j].texcoord;
//	//			pbr_shader->attributes_[j].normal_os = model->attributes_[i + j].normal_os;
//	//			pbr_shader->attributes_[j].tangent_os = model->attributes_[i + j].tangent_os;
//	//			break;
//	//		}
//	//	}
//	//	// 绘制三角形
//	//	renderer->DrawMesh();
//	//}
//#pragma endregion
//#pragma region 渲染Skybox
//		scene->UpdateShaderInfo(skybox_shader);
//		renderer->SetVertexShader(skybox_shader->vertex_shader_);
//		renderer->SetPixelShader(skybox_shader->pixel_shader_);
//
//		camera->UpdateSkyBoxUniformBuffer(skybox_shader->uniform_buffer_);
//		camera->HandleInputEvents();
//		camera->UpdateSkyboxMesh(skybox_shader);
//		for (size_t i = 0; i < skybox_shader->plane_vertex_.size() - 2; i++)
//		{
//			skybox_shader->attributes_[0].position_os = skybox_shader->plane_vertex_[0];
//			skybox_shader->attributes_[1].position_os = skybox_shader->plane_vertex_[i + 1];
//			skybox_shader->attributes_[2].position_os = skybox_shader->plane_vertex_[i + 2];
//
//			renderer->DrawSkybox();
//		}
//#pragma endregion
//		window->WindowDisplay(renderer->color_buffer_);
//	}
//#pragma endregion
//
//	//system("pause");
//
//
//}

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





