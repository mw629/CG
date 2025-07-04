#include "../Core/VariableTypes.h"




//三次元ベクトルの計算


/// <summary>
/// この関数は、2つのベクトルの和を計算します。
/// </summary>
/// <param name="v1">ベクトル1</param>
/// <param name="v2">ベクトル2</param>
///  <returns>ベクトルの各要素を足し合わせた結果を返します。 </returns>
Vector3 AddVector3(Vector3 v1, Vector3 v2);

/// <summary>
/// この関数は、2つのベクトルの差を計算します。
/// </summary>
/// <param name="v1">ベクトル1</param>
/// <param name="v2">ベクトル2</param>
///  <returns>ベクトルの各要素を引いた結果を返します。 </returns>
Vector3 SubtractVector3(Vector3 v1, Vector3 v2);

/// <summary>
/// この関数は二つのベクトルの積を計算します
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>ベクトルの各要素を掛けた結果を返します</returns>
Vector3 MultiplyVector3(Vector3 v1, Vector3 v2);

/// <summary>
/// この関数は、指定されたスカラー倍をベクトルの各要素に適用します。
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="s">スカラー値</param>
/// <returns>スカラー倍された新しいベクトル</returns>
Vector3 ScalarMultiply(Vector3 v, float s);

/// <summary>
/// この関数は、ベクトルの長さを計算します。
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ</returns>
float Length(Vector3 v);

/// <summary>
/// この関数は、ベクトルを正規化します。
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>正規化されたベクトル</returns>
Vector3 Normalize(Vector3 v);

/// <summary>
/// この関数は、2つのベクトルの内積を計算します。
/// </summary>
/// <param name="v1">ベクトル1</param>
/// <param name="v2">ベクトル2</param>
/// <returns>内積の結果</returns>
float Dot(Vector3 v1, Vector3 v2);


/// <summary>
/// この関数は、2つのベクトルのクロス積(外積)を計算します。
/// </summary>
/// <param name="v1">ベクトル1</param>
/// <param name="v2">ベクトル2</param>
/// <returns>クロス積(外積)の結果</returns>
Vector3 Cross(const Vector3& v1, const Vector3& v2);

//行列の計算

/// <summary>
/// この関数は、2つの行列の和を計算します。
/// </summary>
/// <param name="m1">行列1</param>
/// <param name="m2">行列2</param>
/// <returns>2つの行列の和</returns>
Matrix4x4 AddMatrix4x4(Matrix4x4 m1, Matrix4x4 m2);


/// <summary>
/// この関数は、2つの行列の積を計算します。
/// </summary>
/// <param name="m1">行列1</param>
/// <param name="m2">行列2</param>
/// <returns>2つの行列の積</returns>
Matrix4x4 MultiplyMatrix4x4(Matrix4x4 m1, Matrix4x4 m2);

/// <summary>
/// この関数は、行列にスカラー倍を適用します。
/// </summary>
/// <param name="m">行列</param>
/// <param name="s">スカラー値</param>
/// <returns>スカラー倍された行列</returns>
Matrix4x4 ScaleMultiplyMatrix4x4(Matrix4x4 m, float s);

/// <summary>
/// この関数は、行列の転置を計算します。
/// </summary>
/// <param name="m">行列</param>
/// <returns>転置された行列</returns>
Matrix4x4 TransposeMatrix4x4(Matrix4x4 m);




/// <summary>
/// この関数は、行列の逆行列を計算します。
/// </summary>
/// <param name="m">行列</param>
/// <returns>逆行列</returns>
Matrix4x4 Inverse(Matrix4x4 m);

/// <summary>
/// この関数は、ベクトルを行列で変換します。
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="m">行列</param>
/// <returns>変換されたベクトル</returns>
Vector3 TransformMatrix(const Vector3& v, const Matrix4x4& m);


/// <summary>
/// 単位行列を出します
/// </summary>
Matrix4x4 IdentityMatrix();

///アフィン行列

/// <summary>
/// この関数は、行列をスケーリングします。
/// </summary>
/// <param name="scall">スケーリング値</param>
/// <returns>スケーリング行列</returns>
Matrix4x4 Scale(Vector3 scall);

/// <summary>
/// この関数は、X軸を中心に回転する行列を計算します。
/// </summary>
/// <param name="angle">回転角度</param>
/// <returns>回転行列</returns>
Matrix4x4 RotationX(float angle);

/// <summary>
/// この関数は、Y軸を中心に回転する行列を計算します。
/// </summary>
/// <param name="angle">回転角度</param>
/// <returns>回転行列</returns>
Matrix4x4 RotationY(float angle);

/// <summary>
/// この関数は、Z軸を中心に回転する行列を計算します。
/// </summary>
/// <param name="angle">回転角度</param>
/// <returns>回転行列</returns>
Matrix4x4 RotationZ(float angle);

/// <summary>
/// この関数は、任意の軸を中心に回転する行列を計算します。
/// </summary>
/// <param name="angleX">X軸回転角度</param>
/// <param name="angleY">Y軸回転角度</param>
/// <param name="angleZ">Z軸回転角度</param>
/// <returns>回転行列</returns>
Matrix4x4 Rotation(Vector3 angle);//不安

/// <summary>
/// この関数は、ベクトルを行列で変換します。
/// </summary>
/// <param name="pos">位置</param>
/// <param name="m">行列</param>
Matrix4x4 Translation(Vector3 pos);

Vector3 MakeWorldPos(PolarCoordinates pos);

Vector3 MakeWorldROteta(PolarCoordinates pos);

Matrix4x4 MakeLookAtMatrix(
    const Vector3& eye,     // カメラの位置
    const Vector3& target,  // 見たい場所
    const Vector3& up       // 上方向ベクトル（たいてい {0,1,0}）
);

Matrix4x4 IdentityMatrix();

/// <summary>
/// この関数は、アフィン行列を作成します。
/// </summary>
/// <param name="pos">位置</param>
/// <param name="scale">スケーリング値</param>
/// <param name="angle">回転角度</param>
/// <returns>アフィン行列</returns>
Matrix4x4 MakeAffineMatrix(Vector3 pos, Vector3 scale, Vector3 angle);


Matrix4x4 MakeAffineMatrix(Matrix4x4 translationMatrix, Vector3 scale, Matrix4x4 rotationMatrix);

/// <summary>
/// この関数は、正射影行列を作成します。
/// </summary>
/// <param name="l">左</param>
/// <param name="r">右</param>
/// <param name="t">上</param>
/// <param name="b">下</param>
/// <param name="zn">近クリップ面</param>
/// <param name="zf">遠クリップ面</param>
/// <returns>正射影行列</returns>
Matrix4x4 MakeOrthographicMatrix(float l, float r, float t, float b, float zn, float zf);

/// <summary>
/// この関数は、透視投影行列を作成します。
/// </summary>
/// <param name="fov">視野角</param>
/// <param name="aspect">アスペクト比</param>
/// <param name="zn">近クリップ面</param>
/// <param name="zf">遠クリップ面</param>
/// <returns>透視投影行列</returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float a, float zn, float zf);

/// <summary>
/// この関数は、ビューポート行列を作成します。
/// </summary>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="left">左</param>
/// <param name="top">上</param>
/// <param name="minD">最小深度値</param>
/// <param name="maxD">最大深度値</param>
/// <returns>ビューポート行列</returns>
Matrix4x4 MakeViewPortMatrix(float width, float height, float left, float top, float minD, float maxD);


Matrix4x4 MakeLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up);