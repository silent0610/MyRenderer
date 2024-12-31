﻿#pragma once

#include "vector.h"

#pragma region 矩阵定义

template<size_t ROW, size_t COL, typename T>struct Matrix {
	T m[ROW][COL];
	inline Matrix() {}; //此时每个元素未被初始化

	//相同大小的矩阵初始化
	inline Matrix(const Matrix<ROW, COL, T>& src) {
		for (size_t i = 0; i < ROW; i++)
			for (size_t j = 0; j < COL; j++)
				m[i][j] = src.m[i][j];
	}

	inline Matrix(const std::initializer_list<Vector<COL, T>>& matrix) {
		auto it = matrix.begin();
		for (size_t i = 0; i < ROW; i++) SetRow(i, *it++);  //用initializer_list初始化每一行
	}

	inline const T* operator[](size_t row)const { assert(row < ROW); return m[row]; }
	inline T* operator [] (size_t row) { assert(row < ROW); return m[row]; }

	inline Vector<COL, T> GetRow(size_t row)const {
		assert(row < ROW);
		Vector<COL, T> v;
		for (size_t i = 0; i < COL; i++) v[i] = m[row][i];
		return v;
	}

	inline Vector<ROW, T> GetCol(size_t col) const {
		assert(col < COL);
		Vector<ROW, T> v;
		for (size_t i = 0; i < ROW; i++) v[i] = m[i][col];
		return v;
	}

	inline void SetRow(size_t row, const Vector<COL, T>&v)  {
		assert(row < ROW);
		for (size_t i = 0; i < COL; i++) m[row][i] = v[i];
	}
	inline void SetCol(size_t col, const Vector<ROW, T>&v) {
		assert(col < COL);
		for (size_t i = 0; i < ROW; i++) m[i][col] = v[i];
	}

	// 取得删除某行和某列的子矩阵：子式
	inline Matrix<ROW - 1, COL - 1, T> GetMinor(size_t row, size_t col) const {
		Matrix<ROW - 1, COL - 1, T> ret;
		for (size_t r = 0; r < ROW - 1; r++) {
			for (size_t c = 0; c < COL - 1; c++) {
				ret.m[r][c] = m[r < row ? r : r + 1][c < col ? c : c + 1];
			}
		}
		return ret;
	}

	// 取得转置矩阵
	inline Matrix<COL, ROW, T> Transpose() const {
		Matrix<COL, ROW, T> ret;
		for (size_t r = 0; r < ROW; r++) {
			for (size_t c = 0; c < COL; c++)
				ret.m[c][r] = m[r][c];
		}
		return ret;
	}

	// 全0矩阵，静态方法
	inline static Matrix<ROW, COL, T> GetZero() {
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++) {
			for (size_t c = 0; c < COL; c++)
				ret.m[r][c] = 0;
		}
		return ret;
	}

	// 静态 单位（全1）矩阵，由于二维数组不会自己初始化,需要手动初始化
	inline static Matrix<ROW, COL, T> GetIdentity() {
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++) {
			for (size_t c = 0; c < COL; c++)
				ret.m[r][c] = (r == c) ? 1 : 0;
		}
		return ret;
	}
};
#pragma endregion
#pragma region 矩阵运算
// 矩阵采用行主序，矩阵乘法使用"左乘"

// 判断两个矩阵是否相等
template<size_t ROW, size_t COL, typename T>
inline bool operator == (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	for (size_t r = 0; r < ROW; r++)
		for (size_t c = 0; c < COL; c++)
			if (a.m[r][c] != b.m[r][c]) return false;

	return true;
}
// 判断两个矩阵是否不等
template<size_t ROW, size_t COL, typename T>
inline bool operator != (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	for (size_t r = 0; r < ROW; r++)
		for (size_t c = 0; c < COL; c++)
			if (a.m[r][c] != b.m[r][c]) return true;

	return false;
}

// 正号
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator + (const Matrix<ROW, COL, T>& src) {
	return src;
}

// 负号
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator - (const Matrix<ROW, COL, T>& src) {
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			out.m[j][i] = -src.m[j][i];

	return out;
}

// 两个相同大小的矩阵相加
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator + (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			ans.m[j][i] = a.m[j][i] + b.m[j][i];

	return ans;
}

// 两个相同大小的矩阵相减
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator - (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			ans.m[j][i] = a.m[j][i] - b.m[j][i];

	return ans;
}

// 两个矩阵对应元素相乘
template<size_t ROW, size_t COL, size_t NEWCOL, typename T>
inline Matrix<ROW, NEWCOL, T> operator * (const Matrix<ROW, COL, T>& a, const Matrix<COL, NEWCOL, T>& b) {
	Matrix<ROW, NEWCOL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < NEWCOL; i++)
			ans.m[j][i] = vector_dot(a.GetRow(j), b.GetCol(i)); // 对应向量的点乘

	return ans;
}

// 矩阵除以标量
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator / (const Matrix<ROW, COL, T>& a, T x) {
	assert(x != 0);
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			ans.m[i][j] = a.m[i][j] / x;

	return ans;
}

// 矩阵左乘常数
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator * (T x, const Matrix<ROW, COL, T>& a) {
	return (a * x);
}

// 常数除以矩阵
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator / (T x, const Matrix<ROW, COL, T>& a) {
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++) {
		for (size_t i = 0; i < COL; i++) {
			ans.m[j][i] = x / a.m[j][i];
		}
	}
	return ans;
}
// 行向量 乘 矩阵 得到 行向量
template<size_t ROW, size_t COL, typename T>
inline Vector<COL, T> operator * (const Vector<ROW, T>& a, const Matrix<ROW, COL, T>& m) {
	Vector<COL, T> b;
	for (size_t i = 0; i < COL; i++)
		b[i] = vector_dot(a, m.Col(i));
	return b;
}

// 矩阵 乘 列向量 得到列向量
template<size_t ROW, size_t COL, typename T>
inline Vector<ROW, T> operator * (const Matrix<ROW, COL, T>& m, const Vector<COL, T>& a) {
	Vector<ROW, T> b;
	for (size_t i = 0; i < ROW; i++)
		b[i] = vector_dot(m.GetRow(i), a);
	return b;
}

#pragma endregion

#pragma region 矩阵函数
// 包括了针对矩阵进行的各种计算，例如行列式求值，余子式，伴随矩阵，逆矩阵等等

// 行列式求值：一阶方阵
template<typename T>
inline T matrix_det(const Matrix<1, 1, T>& m) {
	return m[0][0];
}

// 行列式求值: 二阶方阵
template<typename T>
inline T matrix_det(const Matrix<2, 2, T>& m) {
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

// 行列式求值：多阶行列式，即第一行同其余子式相乘求和
template<size_t N, typename T>
inline T matrix_det(const Matrix<N, N, T>& m) {
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += m[0][i] * matrix_cofactor(m, 0, i);
	return sum;
}

// 余子式：一阶
template<typename T>
inline T matrix_cofactor(const Matrix<1, 1, T>& m, size_t row, size_t col) {
	return 0;
}

// 多阶余子式：即删除特定行列的子式的行列式值
template<size_t N, typename T>
inline T matrix_cofactor(const Matrix<N, N, T>& m, size_t row, size_t col) {
	return matrix_det(m.GetMinor(row, col)) * (((row + col) % 2) ? -1 : 1); // +-1
}

// 伴随矩阵：即余子式矩阵的转置
template<size_t N, typename T>
inline Matrix<N, N, T> matrix_adjoint(const Matrix<N, N, T>& m) {
	Matrix<N, N, T> ret;
	for (size_t j = 0; j < N; j++)
		for (size_t i = 0; i < N; i++)
			ret[j][i] = matrix_cofactor(m, i, j);

	return ret;
}

// 求逆矩阵：使用伴随矩阵除以行列式的值得到
template<size_t N, typename T>
inline Matrix<N, N, T> matrix_invert(const Matrix<N, N, T>& m) {
	Matrix<N, N, T> ret = matrix_adjoint(m);
	T det = vector_dot(m.GetRow(0), ret.GetCol(0));
	return ret / det;
}

// 文本流输出
template<size_t ROW, size_t COL, typename T>
inline std::ostream& operator << (std::ostream& os, const Matrix<ROW, COL, T>& m) {
	for (size_t r = 0; r < ROW; r++) {
		Vector<COL, T>row = m.GetRow(r);
		os << row << std::endl;
	}
	return os;
}


#pragma endregion