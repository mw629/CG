#include "Calculation.h"
//#include <math.h>
#include <cmath>
#include "assert.h"






// ベクトル

Vector3 AddVector3(Vector3 v1, Vector3 v2)
{
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vector3 SubtractVector3(Vector3 v1, Vector3 v2)
{
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

Vector3 MultiplyVector3(Vector3 v1, Vector3 v2)
{
	Vector3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

Vector3 ScalarMultiply(Vector3 v, float s)
{
	Vector3 result;
	result = { v.x * s,v.y * s,v.z * s };
	return result;
}

float Lengeh(Vector3 v)
{
	float result;
	result = static_cast<float>(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	return result;
}

Vector3 Normalize(Vector3 v)
{
	Vector3 result{};
	if (Lengeh(v) != 0) {
		result.x = v.x / Lengeh(v);
		result.y = v.y / Lengeh(v);
		result.z = v.z / Lengeh(v);
	}
	return result;
}

float Dot(Vector3 v1, Vector3 v2)
{
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

// 行列

Matrix4x4 AddMatrix4x4(Matrix4x4 m1, Matrix4x4 m2)//o
{
	Matrix4x4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

Matrix4x4 MultiplyMatrix4x4(Matrix4x4 m1, Matrix4x4 m2)//o
{
	Matrix4x4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j]
				= m1.m[i][0] * m2.m[0][j]
					+ m1.m[i][1] * m2.m[1][j]
						+ m1.m[i][2] * m2.m[2][j]
							+ m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
}

Matrix4x4 TransposeMatrix4x4(Matrix4x4 m)//o
{
	Matrix4x4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}

Matrix4x4 Inverse(Matrix4x4 m)
{
	Matrix4x4 result;

	float det
		= m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
		+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
		+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
		- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
		- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
		- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
		- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
		+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
		+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
		+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
		- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
		- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
		- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
		+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
		+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) / det;//o

	result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) / det;//o

	result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) / det;//o

	result.m[0][3] = (-m.m[0][1] * m.m[1][3] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) / det;//o


	result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) / det;//o

	result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) / det;//o

	result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) / det;//o

	result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) / det;//o


	result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) / det;//o

	result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) / det;//o

	result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) / det;//o

	result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][0]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) / det;//o


	result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) / det;//o

	result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) / det;//o

	result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) / det;//o

	result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) / det;//o

	return result;
}

Matrix4x4 ScaleMultiplyMatrix4x4(Matrix4x4 m, float s)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m.m[i][j] * s;
		}
	}
	return result;
}

Vector3 TransformMatrix(const Vector3& v, const Matrix4x4& m)
{
	Vector3 result{};

	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
	if (w != 0) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	return result;
}

Matrix4x4 Scale(Vector3 scall)
{
	Matrix4x4 result;

	result.m[0][0] = 1.0f * scall.x;
	result.m[1][1] = 1.0f * scall.y;
	result.m[2][2] = 1.0f * scall.z;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 RotationX(float angle)
{
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cos(angle);
	result.m[1][2] = std::sin(angle);
	result.m[2][1] = std::sin(-angle);
	result.m[2][2] = std::cos(angle);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 RotationY(float angle)
{
	Matrix4x4 result;
	result.m[0][0] = std::cos(angle);
	result.m[0][2] = std::sin(-angle);
	result.m[1][1] = 1.0f;
	result.m[2][0] = std::sin(angle);
	result.m[2][2] = std::cos(angle);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 RotationZ(float angle)
{
	Matrix4x4 result;
	result.m[0][0] = std::cos(angle);
	result.m[0][1] = std::sin(angle);
	result.m[1][0] = std::sin(-angle);
	result.m[1][1] = std::cos(angle);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 Rotation(Vector3 angle)
{
	Matrix4x4 result;
	result = MultiplyMatrix4x4(RotationX(angle.x), MultiplyMatrix4x4(RotationY(angle.y), RotationZ(angle.z)));
	return result;
}

Matrix4x4 Translation(Vector3 pos)
{
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[3][0] = pos.x;
	result.m[3][1] = pos.y;
	result.m[3][2] = pos.z;

	return result;
}

Matrix4x4 IdentityMatrix()
{
	Matrix4x4 m = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f };
	return m;
}

Matrix4x4 MakeAffineMatrix(Vector3 pos, Vector3 scale, Vector3 angle)
{
	Matrix4x4 result;
	Matrix4x4 scaleMatrix = Scale(scale);
	Matrix4x4 rotationMatrix = Rotation(angle);
	Matrix4x4 translationMatrix = Translation(pos);

	result.m[0][0] = scaleMatrix.m[0][0] * rotationMatrix.m[0][0];
	result.m[0][1] = scaleMatrix.m[0][0] * rotationMatrix.m[0][1];
	result.m[0][2] = scaleMatrix.m[0][0] * rotationMatrix.m[0][2];

	result.m[1][0] = scaleMatrix.m[1][1] * rotationMatrix.m[1][0];
	result.m[1][1] = scaleMatrix.m[1][1] * rotationMatrix.m[1][1];
	result.m[1][2] = scaleMatrix.m[1][1] * rotationMatrix.m[1][2];

	result.m[2][0] = scaleMatrix.m[2][2] * rotationMatrix.m[2][0];
	result.m[2][1] = scaleMatrix.m[2][2] * rotationMatrix.m[2][1];
	result.m[2][2] = scaleMatrix.m[2][2] * rotationMatrix.m[2][2];

	result.m[3][0] = translationMatrix.m[3][0];
	result.m[3][1] = translationMatrix.m[3][1];
	result.m[3][2] = translationMatrix.m[3][2];
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MakeOrthographicMatrix(float l, float r, float t, float b, float zn, float zf)
{
	Matrix4x4 result;
	result.m[0][0] = 2.0f / (r - l);
	result.m[1][1] = 2.0f / (t - b);
	result.m[2][2] = 1.0f / (zf - zn);
	result.m[3][0] = (l + r) / (l - r);
	result.m[3][1] = (t + b) / (b - t);
	result.m[3][2] = (zn) / (zn - zf);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float a, float zn, float zf)
{
	Matrix4x4 result;
	result.m[0][0] = static_cast<float>(1.0f / a * (1.0f / tan(fovY / 2.0f)));
	result.m[1][1] = static_cast<float>(1.0f / tan(fovY / 2.0f));
	result.m[2][2] = static_cast<float>(zf / (zf - zn));
	result.m[2][3] = 1.0f;
	result.m[3][2] = static_cast<float>(-zn * zf / (zf - zn));
	return result;
}

Matrix4x4 MakeViewPortMatrix(float width, float height, float left, float top, float minD, float maxD)
{
	Matrix4x4 result;
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxD - minD;
	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minD;
	result.m[3][3] = 1.0f;
	return result;
}

