#pragma once

#include "Vector.h"

#pragma region ������

template<size_t ROW, size_t COL, typename T>struct Matrix {
	T m[ROW][COL];
	inline Matrix() {}; //��ʱÿ��Ԫ��δ����ʼ��

	//��ͬ��С�ľ����ʼ��
	inline Matrix(const Matrix<ROW, COL, T>& src) {
		for (size_t i = 0; i < ROW; i++)
			for (size_t j = 0; j < COL; j++)
				m[i][j] = src.m[i][j];
	}

	inline Matrix(const std::initializer_list<Vector<COL, T>>& matrix) {
		auto it = matrix.begin();
		for (size_t i = 0; i < ROW; i++) SetRow(i, *it++);  //��initializer_list��ʼ��ÿһ��
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

	// ȡ��ɾ��ĳ�к�ĳ�е��Ӿ�����ʽ
	inline Matrix<ROW - 1, COL - 1, T> GetMinor(size_t row, size_t col) const {
		Matrix<ROW - 1, COL - 1, T> ret;
		for (size_t r = 0; r < ROW - 1; r++) {
			for (size_t c = 0; c < COL - 1; c++) {
				ret.m[r][c] = m[r < row ? r : r + 1][c < col ? c : c + 1];
			}
		}
		return ret;
	}

	// ȡ��ת�þ���
	inline Matrix<COL, ROW, T> Transpose() const {
		Matrix<COL, ROW, T> ret;
		for (size_t r = 0; r < ROW; r++) {
			for (size_t c = 0; c < COL; c++)
				ret.m[c][r] = m[r][c];
		}
		return ret;
	}

	// ȫ0���󣬾�̬����
	inline static Matrix<ROW, COL, T> GetZero() {
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++) {
			for (size_t c = 0; c < COL; c++)
				ret.m[r][c] = 0;
		}
		return ret;
	}

	// ��̬ ��λ��ȫ1���������ڶ�ά���鲻���Լ���ʼ��,��Ҫ�ֶ���ʼ��
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
#pragma region ��������
// ������������򣬾���˷�ʹ��"���"

// �ж����������Ƿ����
template<size_t ROW, size_t COL, typename T>
inline bool operator == (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	for (size_t r = 0; r < ROW; r++)
		for (size_t c = 0; c < COL; c++)
			if (a.m[r][c] != b.m[r][c]) return false;

	return true;
}
// �ж����������Ƿ񲻵�
template<size_t ROW, size_t COL, typename T>
inline bool operator != (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	for (size_t r = 0; r < ROW; r++)
		for (size_t c = 0; c < COL; c++)
			if (a.m[r][c] != b.m[r][c]) return true;

	return false;
}

// ����
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator + (const Matrix<ROW, COL, T>& src) {
	return src;
}

// ����
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator - (const Matrix<ROW, COL, T>& src) {
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			out.m[j][i] = -src.m[j][i];

	return out;
}

// ������ͬ��С�ľ������
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator + (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			ans.m[j][i] = a.m[j][i] + b.m[j][i];

	return ans;
}

// ������ͬ��С�ľ������
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator - (const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b) {
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			ans.m[j][i] = a.m[j][i] - b.m[j][i];

	return ans;
}

// ���������ӦԪ�����
template<size_t ROW, size_t COL, size_t NEWCOL, typename T>
inline Matrix<ROW, NEWCOL, T> operator * (const Matrix<ROW, COL, T>& a, const Matrix<COL, NEWCOL, T>& b) {
	Matrix<ROW, NEWCOL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < NEWCOL; i++)
			ans.m[j][i] = vector_dot(a.GetRow(j), b.GetCol(i)); // ��Ӧ�����ĵ��

	return ans;
}

// ������Ա���
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator / (const Matrix<ROW, COL, T>& a, T x) {
	assert(x != 0);
	Matrix<ROW, COL, T> ans;
	for (size_t j = 0; j < ROW; j++)
		for (size_t i = 0; i < COL; i++)
			ans.m[i][j] = a.m[i][j] / x;

	return ans;
}

// ������˳���
template<size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator * (T x, const Matrix<ROW, COL, T>& a) {
	return (a * x);
}

// �������Ծ���
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
// ������ �� ���� �õ� ������
template<size_t ROW, size_t COL, typename T>
inline Vector<COL, T> operator * (const Vector<ROW, T>& a, const Matrix<ROW, COL, T>& m) {
	Vector<COL, T> b;
	for (size_t i = 0; i < COL; i++)
		b[i] = vector_dot(a, m.Col(i));
	return b;
}

// ���� �� ������ �õ�������
template<size_t ROW, size_t COL, typename T>
inline Vector<ROW, T> operator * (const Matrix<ROW, COL, T>& m, const Vector<COL, T>& a) {
	Vector<ROW, T> b;
	for (size_t i = 0; i < ROW; i++)
		b[i] = vector_dot(m.GetRow(i), a);
	return b;
}

#pragma endregion

#pragma region ������
// ��������Ծ�����еĸ��ּ��㣬��������ʽ��ֵ������ʽ��������������ȵ�

// ����ʽ��ֵ��һ�׷���
template<typename T>
inline T matrix_det(const Matrix<1, 1, T>& m) {
	return m[0][0];
}

// ����ʽ��ֵ: ���׷���
template<typename T>
inline T matrix_det(const Matrix<2, 2, T>& m) {
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

// ����ʽ��ֵ���������ʽ������һ��ͬ������ʽ������
template<size_t N, typename T>
inline T matrix_det(const Matrix<N, N, T>& m) {
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += m[0][i] * matrix_cofactor(m, 0, i);
	return sum;
}

// ����ʽ��һ��
template<typename T>
inline T matrix_cofactor(const Matrix<1, 1, T>& m, size_t row, size_t col) {
	return 0;
}

// �������ʽ����ɾ���ض����е���ʽ������ʽֵ
template<size_t N, typename T>
inline T matrix_cofactor(const Matrix<N, N, T>& m, size_t row, size_t col) {
	return matrix_det(m.GetMinor(row, col)) * (((row + col) % 2) ? -1 : 1); // +-1
}

// ������󣺼�����ʽ�����ת��
template<size_t N, typename T>
inline Matrix<N, N, T> matrix_adjoint(const Matrix<N, N, T>& m) {
	Matrix<N, N, T> ret;
	for (size_t j = 0; j < N; j++)
		for (size_t i = 0; i < N; i++)
			ret[j][i] = matrix_cofactor(m, i, j);

	return ret;
}

// �������ʹ�ð�������������ʽ��ֵ�õ�
template<size_t N, typename T>
inline Matrix<N, N, T> matrix_invert(const Matrix<N, N, T>& m) {
	Matrix<N, N, T> ret = matrix_adjoint(m);
	T det = vector_dot(m.GetRow(0), ret.GetCol(0));
	return ret / det;
}

// �ı������
template<size_t ROW, size_t COL, typename T>
inline std::ostream& operator << (std::ostream& os, const Matrix<ROW, COL, T>& m) {
	for (size_t r = 0; r < ROW; r++) {
		Vector<COL, T>row = m.GetRow(r);
		os << row << std::endl;
	}
	return os;
}


#pragma endregion