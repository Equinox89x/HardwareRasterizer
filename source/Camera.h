#pragma once
#include "Vector3.h"
#include "directxmath.h"

using namespace dae;
class Camera
{

public:

	Camera() = default;
	Camera(float fovAngle, Vector3 origin);

	float m_FovAngle{ 90.f };
	float m_Fov{ };

	const float nearZ{ 0.1f };
	const float farZ{ 100.f };

	float m_AspectRatio{ 0.f };
	Matrix m_WorldViewProjectionMatrix{};
	Matrix m_InvViewMatrix{};
	Matrix m_OnbMatrix{};
	Matrix m_ViewMatrix{};
	Vector3 m_Origin{ 0,0,0 };

	Matrix GetViewMatrix() { return m_ViewMatrix; };
	Matrix GetProjectionMatrix() { return m_ProjectionMatrix; };
	void Update(float deltaTime);

private:
	void CalculateViewMatrix();
	void CalculateProjectionMatrix();

	const float m_MovementSpeed{ 5.f };

	Vector3 m_Forward{ 0.266f, -0.453f, 0.860f };
	Vector3 m_Up{ Vector3::UnitY };
	Vector3 m_Right{ Vector3::UnitX };
	Matrix m_ProjectionMatrix{};
};

