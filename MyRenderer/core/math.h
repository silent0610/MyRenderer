#pragma once
#include "Matrix.h"
#include <cmath>


constexpr float kPi = 3.1415926f;
constexpr float kEpsilon = 1e-5f;

// �������ͱ���
typedef Vector<2, float>  Vec2f;
typedef Vector<2, double> Vec2d;
typedef Vector<2, int>    Vec2i;
typedef Vector<3, float>  Vec3f;
typedef Vector<3, double> Vec3d;
typedef Vector<3, int>    Vec3i;
typedef Vector<4, float>  Vec4f, ColorRGBA;
typedef Vector<4, double> Vec4d;
typedef Vector<4, int>    Vec4i;

// ʹ��ColorRGBA�����ɫ����ʹ��ColorRGBA_32bit���ͼ�����
// uint8_t ��λ�޷�������
typedef Vector<4, uint8_t> ColorRGBA32Bit;

// �������ͱ���
typedef Matrix<4, 4, float> Mat4x4f;
typedef Matrix<3, 3, float> Mat3x3f;
typedef Matrix<4, 3, float> Mat4x3f;
typedef Matrix<3, 4, float> Mat3x4f;

#pragma region ������ѧ����

template<typename T> inline T Abs(T x) { return (x < 0) ? (-x) : x; }
template<typename T> inline T Max(T x, T y) { return (x < y) ? y : x; }
template<typename T> inline T Min(T x, T y) { return (x > y) ? y : x; }

template<typename T> inline bool NearEqual(T x, T y, T error) {
	return (Abs(x - y) < error);
}

template<typename T>inline T Between(T min_x, T max_x, T x) {
	return Min(Max(min_x, x), max_x);
}

template<typename T> inline T Saturate(T x) {
	return Between(T(0), T(1), x);
}
#pragma endregion

#pragma region 3D ��ѧ����


#pragma endregion