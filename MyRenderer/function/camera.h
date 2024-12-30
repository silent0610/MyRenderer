#pragma once

#include "../core/math.h"
#include "shader.h"
#include "../core/window.h"

class Camera
{
public:
	Camera(const Vec3f& positon, const Vec3f& target, const Vec3f& up, float fov, float aspect);
	~Camera() = default;
	
	// ���������¼�
	void HandleInputEvents();

	// ����uniform buffer�еľ���
	void UpdateUniformBuffer(UniformBuffer* uniform_buffer, const Mat4x4f& model_matrix) const;
	void UpdateSkyBoxUniformBuffer(UniformBuffer* uniform_buffer) const;
	void UpdateSkyboxMesh(SkyBoxShader* sky_box_shader) const;

	Window* window_;
	Vec3f position_; // ���������ռ�λ��
	Vec3f origin_position_;	//�����λ��,���ڸ�λ
	Vec3f target_;// ������������ռ�λ��
	Vec3f origin_target_;	//�����Ŀ��λ��,���ڸ�λ
	Vec3f up_;							// �����up����

	/*
		�������ϵ����
		axis_r��������ָ����Ļ�Ҳ�
		axis_u��������ָ����Ļ�ϲ�
		axis_v��������ָ����Ļ
	*/
	Vec3f axis_r_, axis_u_, axis_v_; // �ظ�����?

	float fov_;						// ���FOV
	float aspect_;					// �����

	float near_plane_;				// ���ü�ƽ��
	float far_plane_;				// Զ�ü�ƽ��

private:
	// ���������̬
	void UpdateCameraPose();
	void HandleMouseEvents(); //�������
	void HandleKeyEvents(); // ��������
};