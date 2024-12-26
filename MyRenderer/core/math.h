#pragma once
#include "Matrix.h"
#include <cmath>


constexpr float kPi = 3.1415926f;
constexpr float kEpsilon = 1e-5f;

// 向量类型别名
typedef Vector<2, float>  Vec2f;
typedef Vector<2, double> Vec2d;
typedef Vector<2, int>    Vec2i;
typedef Vector<3, float>  Vec3f;
typedef Vector<3, double> Vec3d;
typedef Vector<3, int>    Vec3i;
typedef Vector<4, float>  Vec4f, ColorRGBA;
typedef Vector<4, double> Vec4d;
typedef Vector<4, int>    Vec4i;

// 使用ColorRGBA完成颜色计算使用ColorRGBA_32bit完成图像输出
// uint8_t 八位无符号整数
typedef Vector<4, uint8_t> ColorRGBA32Bit;

// 矩阵类型别名
typedef Matrix<4, 4, float> Mat4x4f;
typedef Matrix<3, 3, float> Mat3x3f;
typedef Matrix<4, 3, float> Mat4x3f;
typedef Matrix<3, 4, float> Mat3x4f;

#pragma region 基本数学函数

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

#pragma region 3D 数学运算


#pragma endregion