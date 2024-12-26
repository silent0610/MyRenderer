#pragma once

#include <assert.h>
#include <initializer_list>
#include <iostream>

#pragma region ��������

//Nά����ģ�� ͨ�õ�
template<size_t n, typename T> struct Vector {
	T m[n]; // nά���� 
	Vector() { for (size_t i = 0; i < n; ++i) m[i] = T(); } //Ĭ�Ϲ��캯�� ��
	explicit Vector(const T* ptr) { for (size_t i = 0; i < n; ++i) m[i] = ptr[i]; } //��ʽʹ��ָ���ʼ��, ����ָ��n��Ԫ�ص�m��
	Vector(const Vector<n, T>& v) { for (size_t i = 0; i < n; ++i)m[i] = v.m[i]; } //��������
	Vector(const std::initializer_list<T>& list) { auto it = list.begin(); for (size_t i = 0; i < n; i++) m[i] = *it++; }//��ʼ���б���
	const T& operator[](size_t i) const { assert(i < n); return m[i]; } //��������[]�����,ͬʱʹ��assert����Ƿ�Խ��
	T& operator[](size_t i) { assert(i < n); return m[i]; } //����[]�����,ͬʱʹ��assert����Ƿ�Խ��
	void load(const T* ptr) { for (size_t i = 0; i < n; i++)m[i] = ptr[i]; }; //��������ָ��n��Ԫ�ص�m��
	void save(T* ptr) { for (size_t i = 0; i < n; i++)ptr[i] = m[i]; }; //��������m��n��Ԫ�ص�ָ����
};

//2ά����ģ�� �ػ�
template<typename T> struct Vector<2, T> {
	union {
		struct { T x, y; };//����
		struct { T u, v; };
		T m[2];
	};
	inline Vector() :x(T()), y(T()) {} //Ĭ�Ϲ��캯�� ��
	inline Vector(T value) : x(value), y(value) {} //���캯��
	inline Vector(T x, T y) : x(x), y(y) {} //���캯��
	inline Vector(const Vector<2, T>& v) : x(v.x), y(v.y) {} //��������
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]) {} //ʹ��ָ���ʼ��, ����ָ��2��Ԫ�ص�m��
	inline const T& operator[](size_t i)const { assert(i < 2); return m[i]; } //��������[]�����,ͬʱʹ��assert����Ƿ�Խ��
	inline T& operator[](size_t i) { assert(i < 2); return m[i]; } //����[]�����,ͬʱʹ��assert����Ƿ�Խ��
	inline void load(const T* ptr) { x = ptr[0]; y = ptr[1]; } //��������ָ��2��Ԫ�ص�m��
	inline void save(T* ptr) { ptr[0] = x; ptr[1] = y; } //��������m��2��Ԫ�ص�ָ����

	inline Vector<2, T> xy() const { return *this; } //��������
	inline Vector<3, T> xy1() const { return Vector<3, T>(x, y,1); } //����x,y,1
	inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, 1, 1); } //����x,y,1,1
};

//3ά����ģ�� �ػ�
template<typename T> struct Vector<3, T> {
	union {
		struct { T x, y, z; };//����
		struct { T r, g, b; };
		T m[3];
	};
	inline Vector() :x(T()), y(T()), z(T()) {} //Ĭ�Ϲ��캯�� ��
	inline Vector(T value) : x(value), y(value),z(value) {} //���캯��
	inline Vector(T x, T y,T z) : x(x), y(y),z(z) {} //���캯��
	inline Vector(const Vector<3, T>& v) : x(v.x), y(v.y), z(v.z) {} //��������
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]),z(ptr[2]) {} //ʹ��ָ���ʼ��, ����ָ��2��Ԫ�ص�m��
	inline const T& operator[](size_t i)const { assert(i < 3); return m[i]; } //��������[]�����,ͬʱʹ��assert����Ƿ�Խ��
	inline T& operator[](size_t i) { assert(i < 3); return m[i]; } //����[]�����,ͬʱʹ��assert����Ƿ�Խ��
	inline void load(const T* ptr) { x = ptr[0]; y = ptr[1]; z = ptr[2]; } //��������ָ��2��Ԫ�ص�m��
	inline void save(T* ptr) { ptr[0] = x; ptr[1] = y; ptr[2] = z; } //��������m��2��Ԫ�ص�ָ����

	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); } //����x,y
	inline Vector<3, T> xyz() const { return *this; } //��������
	inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, z, 1); } //����x,y,z,1
};
//4ά����ģ�� �ػ�
template<typename T> struct Vector<4, T> {
	union {
		struct { T x, y, z, w; };//����
		struct { T r, g, b, a; };
		T m[4];
	};
	inline Vector() :x(T()), y(T()), z(T()),w(T()) {} //Ĭ�Ϲ��캯�� ��
	inline Vector(T value) : x(value), y(value), z(value) ,w(value){} //���캯��
	inline Vector(T x, T y, T z,T w) : x(x), y(y), z(z) ,w(w){} //���캯��
	inline Vector(const Vector<4, T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {} //��������
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]) ,w(ptr[3]) {} //ʹ��ָ���ʼ��, ����ָ��2��Ԫ�ص�m��
	inline const T& operator[](size_t i)const { assert(i < 4); return m[i]; } //��������[]�����,ͬʱʹ��assert����Ƿ�Խ��
	inline T& operator[](size_t i) { assert(i < 4); return m[i]; } //����[]�����,ͬʱʹ��assert����Ƿ�Խ��
	inline void load(const T* ptr) { x = ptr[0]; y = ptr[1]; z = ptr[2]; w = ptr[3];	} //��������ָ��2��Ԫ�ص�m��
	inline void save(T* ptr) { ptr[0] = x; ptr[1] = y; ptr[2] = z; ptr[3] = w;	} //��������m��2��Ԫ�ص�ָ����

	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); } //����x,y
	inline Vector<3, T> xyz() const { return Vector<3, T>(x, y,z);} //����x,y,z
	inline Vector<4, T> xy11() const { return *this; } //��������
};

#pragma endregion

#pragma region ��������
// ����������������֮�䳣����������������

// (+a) ����
template <size_t N, typename T>
inline Vector<N, T> operator + (const Vector<N, T>& a) {
	return a;
}

// (-a) ����
template <size_t N, typename T>
inline Vector<N, T> operator - (const Vector<N, T>& a) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = -a[i];
	return b;
}

// (a == b)���ж����
template <size_t N, typename T>
inline bool operator == (const Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) if (a[i] != b[i]) return false;
	return true;
}

// (a != b)? �жϲ���
template <size_t N, typename T>
inline bool operator != (const Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) if (a[i] != b[i]) return true;
	return false;
}

// (a + b) ���
template <size_t N, typename T>
inline Vector<N, T> operator + (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> sum;
	for (size_t i = 0; i < N; i++) sum[i] = a[i] + b[i];
	return sum;
}

// (a - b) ���
template <size_t N, typename T>
inline Vector<N, T> operator - (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> diff;
	for (size_t i = 0; i < N; i++) diff[i] = a[i] - b[i];
	return diff;
}

// (a * b)�����ǵ��Ҳ���ǲ�ˣ����Ǹ���Ԫ�طֱ���ˣ�ɫ�ʼ���ʱ����
template <size_t N, typename T>
inline Vector<N, T> operator * (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = a[i] * b[i];
	return c;
}

// (a / b)������Ԫ�����
template <size_t N, typename T>
inline Vector<N, T> operator / (const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = a[i] / b[i];
	return c;
}

//��a * x�� ����һ������, ��������
template <size_t N, typename T>
inline Vector<N, T> operator * (const Vector<N, T>& a, T x) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
	return b;
}

//��x * a������һ������, ��������
template <size_t N, typename T>
inline Vector<N, T> operator * (T x, const Vector<N, T>& a) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
	return b;
}

//��a / x����һ��������
template <size_t N, typename T>
inline Vector<N, T> operator / (const Vector<N, T>& a, T x) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) b[i] = a[i] / x;
	return b;
}

//��x / a����һ������
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

#pragma region ��������
// �����˸����������õĺ���

// = |a| ^ 2 ����ģ��ƽ����
template<size_t N, typename T>
inline T vector_length_square(const Vector<N, T>& a) {
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += a[i] * a[i];
	return sum;
}

// |a| ����ģ��
template<size_t N, typename T>
inline T vector_length(const Vector<N, T>& a) {
	return sqrt(vector_length_square(a));
}

// |a| ����ģ�������ػ� float ���ͣ�ʹ�� sqrtf
template<size_t N>
inline float vector_length(const Vector<N, float>& a) {
	return sqrtf(vector_length_square(a));
}

// a / |a| ��������ģ�����Ӷ���һ��
template<size_t N, typename T>
inline Vector<N, T> vector_normalize(const Vector<N, T>& a) {
	return a / vector_length(a);
}

// �������
template<size_t N, typename T>
inline T vector_dot(const Vector<N, T>& a, const Vector<N, T>& b) {
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += a[i] * b[i];
	return sum;
}

// ��ά������ˣ��õ�����
template<typename T>
inline T vector_cross(const Vector<2, T>& a, const Vector<2, T>& b) {
	return a.x * b.y - a.y * b.x;
}

// ��ά������ˣ��õ�������
template<typename T>
inline Vector<3, T> vector_cross(const Vector<3, T>& a, const Vector<3, T>& b) {
	return Vector<3, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// ��ά������ˣ�ǰ��ά��ˣ���һλ����
template<typename T>
inline Vector<4, T> vector_cross(const Vector<4, T>& a, const Vector<4, T>& b) {
	return Vector<4, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w);
}

// ���㷴������, ���淨��n�������������õ���������
template<typename T>
inline Vector<3, T> vector_reflect(const Vector<3, T>& v, const Vector<3, T>& n) {
	return 2.0f * vector_dot(v, n) * n - v;
}

// a + (b - a) * t ������ֵ
template<size_t N, typename T>
inline Vector<N, T> vector_lerp(const Vector<N, T>& a, const Vector<N, T>& b, float t) {
	return a + (b - a) * t;
}

// ����Ԫ��ȡ����ֵ
template<size_t N, typename T>
inline Vector<N, T> vector_abs(const Vector<N, T>& a) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = std::abs(a[i]);
	return c;
}

// ����Ԫ��ȡ�������ֵ
template<size_t N, typename T>
inline Vector<N, T> vector_max(const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = (a[i] > b[i]) ? a[i] : b[i];
	return c;
}
// ����Ԫ��ȡ������Сֵ
template<size_t N, typename T>
inline Vector<N, T> vector_min(const Vector<N, T>& a, const Vector<N, T>& b) {
	Vector<N, T> c;
	for (size_t i = 0; i < N; i++) c[i] = (a[i] < b[i]) ? a[i] : b[i];
	return c;
}

// ��������ֵ������ min_x/max_x ��Χ��
template<size_t N, typename T>
inline Vector<N, T> vector_between(const Vector<N, T>& min_x, const Vector<N, T>& max_x, const Vector<N, T>& x) {
	return vector_min(vector_max(min_x, x), max_x);
}

// �ж���������֮������Ƿ�С��dist
template<size_t N, typename T>
inline bool vector_near(const Vector<N, T>& a, const Vector<N, T>& b, T dist) {
	return (vector_length_square(a - b) <= dist);
}

// �ж����������������Ƿ�������
template<size_t N>
inline bool vector_near_equal(const Vector<N, float>& a, const Vector<N, float>& b, float e = 0.0001) {
	return vector_near(a, b, e);
}

// �ж�����˫���������Ƿ�������
template<size_t N>
inline bool vector_near_equal(const Vector<N, double>& a, const Vector<N, double>& b, double e = 0.0000001) {
	return vector_near(a, b, e);
}

// ����ֵԪ�ط�Χ�ü���0��1
template<size_t N, typename T>
inline Vector<N, T> vector_clamp(const Vector<N, T>& a, T min_x = 0, T max_x = 1) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) {
		T x = (a[i] < min_x) ? min_x : a[i];
		b[i] = (x > max_x) ? max_x : x;
	}
	return b;
}

// ������ı���
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