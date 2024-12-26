#pragma once

#include <assert.h>
#include <initializer_list>
#include <iostream>

#pragma region 向量定义

//N维向量模板 通用的
template<size_t n, typename T> struct Vector {
	T m[n]; // n维向量 
	Vector() { for (size_t i = 0; i < n; ++i) m[i] = T(); } //默认构造函数 空
	explicit Vector(const T* ptr) { for (size_t i = 0; i < n; ++i) m[i] = ptr[i]; } //显式使用指针初始化, 加载指针n个元素到m中
	Vector(const Vector<n, T>& v) { for (size_t i = 0; i < n; ++i)m[i] = v.m[i]; } //拷贝构造
	Vector(const std::initializer_list<T>& list) { auto it = list.begin(); for (size_t i = 0; i < n; i++) m[i] = *it++; }//初始化列表构造
	const T& operator[](size_t i) const { assert(i < n); return m[i]; } //常量重载[]运算符,同时使用assert检查是否越界
	T& operator[](size_t i) { assert(i < n); return m[i]; } //重载[]运算符,同时使用assert检查是否越界
	void load(const T* ptr) { for (size_t i = 0; i < n; i++)m[i] = ptr[i]; }; //主动加载指针n个元素到m中
	void save(T* ptr) { for (size_t i = 0; i < n; i++)ptr[i] = m[i]; }; //主动保存m中n个元素到指针中
};

//2维向量模板 特化
template<typename T> struct Vector<2, T> {
	union {
		struct { T x, y; };//别名
		struct { T u, v; };
		T m[2];
	};
	inline Vector() :x(T()), y(T()) {} //默认构造函数 空
	inline Vector(T value) : x(value), y(value) {} //构造函数
	inline Vector(T x, T y) : x(x), y(y) {} //构造函数
	inline Vector(const Vector<2, T>& v) : x(v.x), y(v.y) {} //拷贝构造
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]) {} //使用指针初始化, 加载指针2个元素到m中
	inline const T& operator[](size_t i)const { assert(i < 2); return m[i]; } //常量重载[]运算符,同时使用assert检查是否越界
	inline T& operator[](size_t i) { assert(i < 2); return m[i]; } //重载[]运算符,同时使用assert检查是否越界
	inline void load(const T* ptr) { x = ptr[0]; y = ptr[1]; } //主动加载指针2个元素到m中
	inline void save(T* ptr) { ptr[0] = x; ptr[1] = y; } //主动保存m中2个元素到指针中

	inline Vector<2, T> xy() const { return *this; } //返回自身
	inline Vector<3, T> xy1() const { return Vector<3, T>(x, y,1); } //返回x,y,1
	inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, 1, 1); } //返回x,y,1,1
};

//3维向量模板 特化
template<typename T> struct Vector<3, T> {
	union {
		struct { T x, y, z; };//别名
		struct { T r, g, b; };
		T m[3];
	};
	inline Vector() :x(T()), y(T()), z(T()) {} //默认构造函数 空
	inline Vector(T value) : x(value), y(value),z(value) {} //构造函数
	inline Vector(T x, T y,T z) : x(x), y(y),z(z) {} //构造函数
	inline Vector(const Vector<3, T>& v) : x(v.x), y(v.y), z(v.z) {} //拷贝构造
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]),z(ptr[2]) {} //使用指针初始化, 加载指针2个元素到m中
	inline const T& operator[](size_t i)const { assert(i < 3); return m[i]; } //常量重载[]运算符,同时使用assert检查是否越界
	inline T& operator[](size_t i) { assert(i < 3); return m[i]; } //重载[]运算符,同时使用assert检查是否越界
	inline void load(const T* ptr) { x = ptr[0]; y = ptr[1]; z = ptr[2]; } //主动加载指针2个元素到m中
	inline void save(T* ptr) { ptr[0] = x; ptr[1] = y; ptr[2] = z; } //主动保存m中2个元素到指针中

	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); } //返回x,y
	inline Vector<3, T> xyz() const { return *this; } //返回自身
	inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, z, 1); } //返回x,y,z,1
};
//4维向量模板 特化
template<typename T> struct Vector<4, T> {
	union {
		struct { T x, y, z, w; };//别名
		struct { T r, g, b, a; };
		T m[4];
	};
	inline Vector() :x(T()), y(T()), z(T()),w(T()) {} //默认构造函数 空
	inline Vector(T value) : x(value), y(value), z(value) ,w(value){} //构造函数
	inline Vector(T x, T y, T z,T w) : x(x), y(y), z(z) ,w(w){} //构造函数
	inline Vector(const Vector<4, T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {} //拷贝构造
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]) ,w(ptr[3]) {} //使用指针初始化, 加载指针2个元素到m中
	inline const T& operator[](size_t i)const { assert(i < 4); return m[i]; } //常量重载[]运算符,同时使用assert检查是否越界
	inline T& operator[](size_t i) { assert(i < 4); return m[i]; } //重载[]运算符,同时使用assert检查是否越界
	inline void load(const T* ptr) { x = ptr[0]; y = ptr[1]; z = ptr[2]; w = ptr[3];	} //主动加载指针2个元素到m中
	inline void save(T* ptr) { ptr[0] = x; ptr[1] = y; ptr[2] = z; ptr[3] = w;	} //主动保存m中2个元素到指针中

	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); } //返回x,y
	inline Vector<3, T> xyz() const { return Vector<3, T>(x, y,z);} //返回x,y,z
	inline Vector<4, T> xy11() const { return *this; } //返回自身
};

#pragma endregion

#pragma region 向量运算
// 囊括了向量、标量之间常见运算符的运算规则

// (+a) 正号
template <size_t N, typename T>
inline Vector<N, T> operator + (const Vector<N, T>& a) {
	return a;
}

// (-a) 负号
template <size_t N, typename T>
inline Vector<N, T> operator - (const Vector<N, T>& a) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = -a[i];
	return b;
}

// (a == b)？判断相等
template <size_t N, typename T>
inline bool operator == (const Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) if (a[i] != b[i]) return false;
	return true;
}

// (a != b)? 判断不等
template <size_t N, typename T>
inline bool operator != (const Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) if (a[i] != b[i]) return true;
	return false;
}

// (a + b) 相加
template <size_t N, typename T>
inline Vector<N, T> operator + (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> sum;
	for (size_t i = 0; i < N; i++) sum[i] = a[i] + b[i];
	return sum;
}

// (a - b) 相减
template <size_t N, typename T>
inline Vector<N, T> operator - (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> diff;
	for (size_t i = 0; i < N; i++) diff[i] = a[i] - b[i];
	return diff;
}

// (a * b)，不是点乘也不是叉乘，而是各个元素分别相乘，色彩计算时有用
template <size_t N, typename T>
inline Vector<N, T> operator * (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = a[i] * b[i];
	return c;
}

// (a / b)，各个元素相除
template <size_t N, typename T>
inline Vector<N, T> operator / (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = a[i] / b[i];
	return c;
}

//（a * x） 乘以一个标量, 标量在右
template <size_t N, typename T>
inline Vector<N, T> operator * (const Vector<N, T>& a, T x) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
	return b;
}

//（x * a）乘以一个标量, 标量在左
template <size_t N, typename T>
inline Vector<N, T> operator * (T x, const Vector<N, T>& a) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
	return b;
}

//（a / x）被一个标量除
template <size_t N, typename T>
inline Vector<N, T> operator / (const Vector<N, T>& a, T x) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = a[i] / x;
	return b;
}

//（x / a）除一个标量
template <size_t N, typename T>
inline Vector<N, T> operator / (T x, const Vector<N, T>& a) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = x / a[i];
	return b;
}

// a += b
template <size_t N, typename T>
inline Vector<N, T>& operator += (Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) a[i] += b[i];
	return a;
}

// a -= b
template <size_t N, typename T>
inline Vector<N, T>& operator -= (Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) a[i] -= b[i];
	return a;
}

// a *= b
template <size_t N, typename T>
inline Vector<N, T>& operator *= (Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) a[i] *= b[i];
	return a;
}

// a /= b
template <size_t N, typename T>
inline Vector<N, T>& operator /= (Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) a[i] /= b[i];
	return a;
}

// a *= x
template <size_t N, typename T>
inline Vector<N, T>& operator *= (Vector<N, T>& a, T x) {
	for (size_t i = 0; i < N; i++) a[i] *= x;
	return a;
}

// a /= x
template <size_t N, typename T>
inline Vector<N, T>& operator /= (Vector<N, T>& a, T x) {
	for (size_t i = 0; i < N; i++) a[i] /= x;
	return a;
}

#pragma endregion

#pragma region 向量函数
// 囊括了各类向量常用的函数

// = |a| ^ 2 向量模长平方和
template<size_t N, typename T>
inline T vector_length_square(const Vector<N, T>& a) {
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += a[i] * a[i];
	return sum;
}

// |a| 向量模长
template<size_t N, typename T>
inline T vector_length(const Vector<N, T>& a) {
	return sqrt(vector_length_square(a));
}

// |a| 向量模长，但特化 float 类型，使用 sqrtf
template<size_t N>
inline float vector_length(const Vector<N, float>& a) {
	return sqrtf(vector_length_square(a));
}

// a / |a| 向量除以模长，从而归一化
template<size_t N, typename T>
inline Vector<N, T> vector_normalize(const Vector<N, T>& a) {
	return a / vector_length(a);
}

// 向量点乘
template<size_t N, typename T>
inline T vector_dot(const Vector<N, T>& a, const Vector<N, T>& b) {
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += a[i] * b[i];
	return sum;
}

// 二维向量叉乘，得到标量
template<typename T>
inline T vector_cross(const Vector<2, T>& a, const Vector<2, T>& b) {
	return a.x * b.y - a.y * b.x;
}

// 三维向量叉乘，得到新向量
template<typename T>
inline Vector<3, T> vector_cross(const Vector<3, T>& a, const Vector<3, T>& b) {
	return Vector<3, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 四维向量叉乘：前三维叉乘，后一位保留
template<typename T>
inline Vector<4, T> vector_cross(const Vector<4, T>& a, const Vector<4, T>& b) {
	return Vector<4, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w);
}

// 计算反射向量, 表面法线n，入射向量，得到出射向量
template<typename T>
inline Vector<3, T> vector_reflect(const Vector<3, T>& v, const Vector<3, T>& n) {
	return 2.0f * vector_dot(v, n) * n - v;
}

// a + (b - a) * t 向量插值
template<size_t N, typename T>
inline Vector<N, T> vector_lerp(const Vector<N, T>& a, const Vector<N, T>& b, float t) {
	return a + (b - a) * t;
}

// 各个元素取绝对值
template<size_t N, typename T>
inline Vector<N, T> vector_abs(const Vector<N, T>& a) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = std::abs(a[i]);
	return c;
}

// 各个元素取二者最大值
template<size_t N, typename T>
inline Vector<N, T> vector_max(const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = (a[i] > b[i]) ? a[i] : b[i];
	return c;
}
// 各个元素取二者最小值
template<size_t N, typename T>
inline Vector<N, T> vector_min(const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = (a[i] < b[i]) ? a[i] : b[i];
	return c;
}

// 将向量的值控制在 min_x/max_x 范围内
template<size_t N, typename T>
inline Vector<N, T> vector_between(const Vector<N, T>& min_x, const Vector<N, T>& max_x, const Vector<N, T>& x) {
	return vector_min(vector_max(min_x, x), max_x);
}

// 判断两个向量之间距离是否小于dist
template<size_t N, typename T>
inline bool vector_near(const Vector<N, T>& a, const Vector<N, T>& b, T dist) {
	return (vector_length_square(a - b) <= dist);
}

// 判断两个单精度向量是否近似相等
template<size_t N>
inline bool vector_near_equal(const Vector<N, float>& a, const Vector<N, float>& b, float e = 0.0001) {
	return vector_near(a, b, e);
}

// 判断两个双精度向量是否近似相等
template<size_t N>
inline bool vector_near_equal(const Vector<N, double>& a, const Vector<N, double>& b, double e = 0.0000001) {
	return vector_near(a, b, e);
}

// 向量值元素范围裁剪到0和1
template<size_t N, typename T>
inline Vector<N, T> vector_clamp(const Vector<N, T>& a, T min_x = 0, T max_x = 1) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) {
		T x = (a[i] < min_x) ? min_x : a[i];
		b[i] = (x > max_x) ? max_x : x;
	}
	return b;
}

// 输出到文本流
template<size_t N, typename T>
inline std::ostream& operator << (std::ostream& os, const Vector<N, T>& a) {
	os << "[";
	size_t i = 0;
	for (; i < N-1; i++) {
		os << a[i];
		os << ", ";
	}
	os << a[i];
	os << "]";
	return os;
}

#pragma endregion