#include "pch.h"
#include "camera.h"
using namespace DirectX;

Camera::Camera(float fovAngle, Vector3 origin) :
	m_FovAngle{fovAngle},
	m_Origin{origin}
{
	m_Fov = tanf((fovAngle * TO_RADIANS) / 2.f);
}

void Camera::Update(float deltaTime)
{
	const Vector3 forwardSpeed{ m_Forward * deltaTime * m_MovementSpeed };
	const Vector3 sideSpeed{ m_Right * deltaTime * m_MovementSpeed };
	const Vector3 upSpeed{ m_Up * deltaTime * m_MovementSpeed };

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	m_Origin += pKeyboardState[SDL_SCANCODE_W] * forwardSpeed;
	m_Origin -= pKeyboardState[SDL_SCANCODE_S] * forwardSpeed;

	m_Origin += pKeyboardState[SDL_SCANCODE_SPACE] * upSpeed;
	m_Origin -= pKeyboardState[SDL_SCANCODE_LCTRL] * upSpeed;

	m_Origin += pKeyboardState[SDL_SCANCODE_D] * sideSpeed;
	m_Origin -= pKeyboardState[SDL_SCANCODE_A] * sideSpeed;

	CalculateViewMatrix();
	CalculateProjectionMatrix();
}

void Camera::CalculateViewMatrix()
{
	//ONB => invViewMatrix
	Vector3 right{ Vector3::Cross(Vector3::UnitY, m_Forward).Normalized() };
	Vector3 upVector{ Vector3::Cross(m_Forward, right).Normalized() };

	Vector4 up4{ upVector, 0 };
	Vector4 right4{ right, 0 };
	Vector4 forward4{ m_Forward, 0 };;
	Vector4 position{ m_Origin, 1 };

	//Inverse(ONB) => ViewMatrix
	Matrix onb{ right4, up4, forward4, position };
	//m_ViewMatrix = onb.Inverse();

	m_ViewMatrix = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up);
	m_InvViewMatrix = m_ViewMatrix.Inverse();
	m_OnbMatrix = onb;
}

void Camera::CalculateProjectionMatrix()
{
	m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_Fov, m_AspectRatio, nearZ, farZ);
}
