#include "Camera.h"
#include "Math/Calculation.h"

Matrix4x4 Camera::MakeWorldViewProjectionMatrix(Transform transform, Transform camera)
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.translate, transform.scale, transform.rotate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera.translate, camera.scale, camera.rotate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(fovY, float(kClientWidth) / float(kClientHeight),zn, zf);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
	Matrix4x4 wvpData = worldViewProjectionMatrix;//camera?
	return wvpData;
}
