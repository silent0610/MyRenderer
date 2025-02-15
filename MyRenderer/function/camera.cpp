﻿#include "camera.h"

//Camera::Camera(const Vec3f& position, const Vec3f& target, const Vec3f& up, float fov, float aspect) :
//	position_(position), target_(target), up_(up), fov_(fov), aspect_(aspect)
//{
//	near_plane_ = 0.4f;
//	//near_plane_ = 1.5f;
//	far_plane_ = 1000.0f;
//
//	origin_position_ = position_;
//	origin_target_ = target_;
//
//	window_ = Window::GetInstance();
//}

//Camera::~Camera() = default;

void Camera::UpdateCameraPose(){
	// 观察向量：从相机位置指向目标位置
	Vec3f view = position_ - target_;
	float radius = vector_length(view);

	float phi = atan2(view[0], view[2]);				// azimuth angle(方位角), z轴和 view的夹角[-pi, pi] ?? //左右
	float theta = acos(view[1] / radius);			// zenith angle(天顶角), view和 y轴的夹角 , [0, pi] //上下
	const float mouse_delta_x = window_->mouse_info_.mouse_delta[0] / window_->width_;
	const float mouse_delta_y = window_->mouse_info_.mouse_delta[1] / window_->height_;

	// 鼠标左键 ,旋转
	if (window_->mouse_buttons_[0])
	{
		constexpr float factor = 1.5 * kPi;

		phi += mouse_delta_x * factor; //左右旋转
		theta += mouse_delta_y * factor; //上下旋转
		if (theta > kPi) theta = kPi - kEpsilon * 100;
		if (theta < 0)  theta = kEpsilon * 100;
	}
	// 平移
	if (window_->mouse_buttons_[1])
	{
		// 鼠标右键
		const float factor = radius * static_cast<float>(tan(60.0 / 360 * kPi)) * 2.2f; //根据距离(position_ - target_)调整平移速度
		
		const Vec3f right = mouse_delta_x * factor * axis_r_; //x方向的平移向量
		const Vec3f up = mouse_delta_y * factor * axis_u_; // y方向的平移向量

		position_ += (-right + up);
		target_ += (-right + up);
	}

	// 鼠标滚轮
	if (window_->mouse_buttons_[2])
	{
		radius *= static_cast<float>(pow(0.95, window_->mouse_info_.mouse_wheel_delta));
		window_->mouse_buttons_[2] = 0;
	}

	// 更新相机位置, 根据视点反推相机位置, 只更新 左键和滚轮, 右键平移在代码块内已更新;
	position_[0] = target_[0] + radius * sin(phi) * sin(theta);
	position_[1] = target_[1] + radius * cos(theta);
	position_[2] = target_[2] + radius * sin(theta) * cos(phi);
}

void Camera::HandleInputEvents()
{
	/*
		计算相机坐标系的轴
		axis_v：观察向量，从目标位置指向相机位置
		axis_r：正方向指向屏幕右侧
		axis_u：正方向指向屏幕上侧
	*/
	axis_v_ = NormalizeVector(target_ - position_);
	axis_r_ = NormalizeVector(vector_cross(axis_v_, up_));
	axis_u_ = NormalizeVector(vector_cross(axis_r_, axis_v_));

	// 处理输入事件
	HandleMouseEvents();
	HandleKeyEvents();
}
void Camera::HandleMouseEvents()
{
	if (window_->mouse_buttons_[0] || window_->mouse_buttons_[1] || window_->mouse_buttons_[2])
	{
		const Vec2f mouse_position = window_->GetMousePosition();
		window_->mouse_info_.mouse_delta = window_->mouse_info_.mouse_pos - mouse_position;
		window_->mouse_info_.mouse_pos = mouse_position;

		UpdateCameraPose();
	}
}
void Camera::HandleKeyEvents()
{
	const float distance = vector_length(target_ - position_);

	if (window_->keys_['Q']) //放大
	{
		const float factor = distance / window_->width_ * 200.0f;
		position_ += -0.05f * axis_v_ * factor;
	}
	if (window_->keys_['E'])//缩小
	{
		position_ += 0.05f * axis_v_;
	}
	if (window_->keys_['W'])
	{
		position_ += 0.05f * axis_u_;
		target_ += 0.05f * axis_u_;
	}
	if (window_->keys_['S'])
	{
		position_ += -0.05f * axis_u_;
		target_ += -0.05f * axis_u_;
	}
	if (window_->keys_['A'])
	{
		position_ += -0.05f * axis_r_;
		target_ += -0.05f * axis_r_;
	}
	if (window_->keys_['D'])
	{
		position_ += 0.05f * axis_r_;
		target_ += 0.05f * axis_r_;
	}
	if (window_->keys_[VK_SPACE])
	{
		position_ = origin_position_;
		target_ = origin_target_;
	}
	if (window_->keys_[VK_ESCAPE])
	{
		window_->is_close_ = true;
	}
}

void Camera::UpdateUniformBuffer(UniformBuffer* uniform_buffer, const Mat4x4f& model_matrix) const
{
	uniform_buffer->model_matrix = model_matrix;
	uniform_buffer->view_matrix = matrix_look_at(position_, target_, up_);
	uniform_buffer->proj_matrix = matrix_set_perspective(fov_, aspect_, near_plane_, far_plane_);;

	uniform_buffer->CalculateRestMatrix();

	uniform_buffer->camera_position = position_;
}
void Camera::UpdateSkyBoxUniformBuffer(UniformBuffer* uniform_buffer) const
{
	uniform_buffer->model_matrix = matrix_set_scale(1.0f, 1.0f, 1.0f);
	uniform_buffer->view_matrix = matrix_look_at(position_, target_, up_);
	uniform_buffer->proj_matrix = matrix_set_perspective(fov_, aspect_, near_plane_, far_plane_);

	uniform_buffer->CalculateRestMatrix();

	uniform_buffer->camera_position = position_;
}
void Camera::UpdateSkyboxMesh(SkyBoxShader* sky_box_shader) const
{
	float fov = fov_ / 180.0f * kPi;
	float yf = tan(fov * 0.5f);
	float  xf = aspect_;

	// 天空盒(天空面)的四个顶点向量
	Vec3f right_top = axis_v_ + axis_r_ * xf + axis_u_ * yf;
	Vec3f left_top = axis_v_ - axis_r_ * xf + axis_u_ * yf;
	Vec3f left_bottom = axis_v_ - axis_r_ * xf - axis_u_ * yf;
	Vec3f right_bottom = axis_v_ + axis_r_ * xf - axis_u_ * yf;

	//Vec3f right_top = Vec3f(xf, yf, 1);
	//Vec3f left_top = Vec3f(-xf, yf, 1);
	//Vec3f left_bottom = Vec3f(-xf, -yf, 1);
	//Vec3f right_bottom = Vec3f(xf, -yf, 1);

	Vec3f camera_position = position_;
	float far_plane = far_plane_ - 2; // 精度 为什么减2?

	sky_box_shader->plane_vertex_[0] = camera_position + far_plane * right_top; //顶点向量乘以远裁剪面z, 得到顶点世界空间坐标
	sky_box_shader->plane_vertex_[1] = camera_position + far_plane * left_top;
	sky_box_shader->plane_vertex_[2] = camera_position + far_plane * left_bottom;
	sky_box_shader->plane_vertex_[3] = camera_position + far_plane * right_bottom;
}


