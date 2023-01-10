#pragma once
#include "Vector3.h"
#include "directxmath.h"

using namespace dae;
struct Camera
{
	Camera() = default;

	Camera(float _fovAngle, const Vector3& _origin);
	void Initialize(float _fovAngle, Vector3 _origin);

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

	float m_TotalPitch{};
	float m_TotalYaw{};

	Matrix GetViewMatrix() { return m_ViewMatrix; };
	Matrix GetProjectionMatrix() { return m_ProjectionMatrix; };
	void Update(float deltaTime);

	void CalculateViewMatrix();
	void CalculateProjectionMatrix();

	const float m_RotationSpeed{ 1.f };
	const float m_MovementSpeed{ 10.f };

	Vector3 m_Forward{ 0.266f, -0.453f, 0.860f };
	Vector3 m_Up{ Vector3::UnitY };
	Vector3 m_Right{ Vector3::UnitX };
	Matrix m_ProjectionMatrix{};
};

