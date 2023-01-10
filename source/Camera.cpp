#include "pch.h"
#include "camera.h"
using namespace DirectX;

Camera::Camera(float _fovAngle, const Vector3& _origin):
	m_Origin{ _origin },
	m_FovAngle{ _fovAngle }
{}

void Camera::Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f })
{
	m_FovAngle = _fovAngle;
	m_Fov = tanf((_fovAngle * TO_RADIANS) / 2.f);

	m_Origin = _origin;
}

void Camera::Update(float deltaTime)
{

	//Camera Update Logic
			//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	const Vector3 forwardSpeed{ m_Forward * deltaTime * m_MovementSpeed };
	const Vector3 sideSpeed{ m_Right * deltaTime * m_MovementSpeed };
	const Vector3 upSpeed{ m_Up * deltaTime * m_MovementSpeed };


	if (SDL_BUTTON(mouseState) == 8) {
		const float rotSpeed{ deltaTime * m_RotationSpeed };
		m_TotalPitch -= static_cast<float>(mouseX) * rotSpeed;
		m_TotalYaw -= static_cast<float>(mouseY) * rotSpeed;
	}
	else if (SDL_BUTTON(mouseState) == 1) {
		m_Origin += static_cast<float>(mouseY) * forwardSpeed;
	}
	else if (SDL_BUTTON(mouseState) == 16) {
		m_Origin += static_cast<float>(mouseY) * upSpeed;
	}

	//reset totalPitch to 0 degrees if it reaches a full spin(360 deg)
	if (m_TotalPitch > 350 || m_TotalPitch < -360) m_TotalPitch = 0;
	Matrix finalRot{ Matrix::CreateRotation(m_TotalYaw, m_TotalPitch, 1) };

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	m_Origin += pKeyboardState[SDL_SCANCODE_W] * forwardSpeed;
	m_Origin -= pKeyboardState[SDL_SCANCODE_S] * forwardSpeed;

	m_Origin += pKeyboardState[SDL_SCANCODE_SPACE] * upSpeed;
	m_Origin -= pKeyboardState[SDL_SCANCODE_LCTRL] * upSpeed;

	m_Origin += pKeyboardState[SDL_SCANCODE_D] * sideSpeed;
	m_Origin -= pKeyboardState[SDL_SCANCODE_A] * sideSpeed;

	m_Forward = finalRot.TransformVector(Vector3::UnitZ);
	m_Forward.Normalize();

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
