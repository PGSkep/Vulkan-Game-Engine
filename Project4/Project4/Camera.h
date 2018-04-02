#ifndef	CAMERA_H
#define CAMERA_H

#include "../../Common/Utility/Math3D.h"
#include "../../Common/Utility/Clock.h"
#include "../../Common/Utility/Input.h"

class Camera
{
public:
	Math3D::Vec3 position = { 0.0f, 0.0f, 0.0f };
	Math3D::Vec3 rotation = { 0.0f, 0.0f, 0.0f };

	float fov, aspect, nearV, farV;

	float speed;

	Math3D::Mat4 GetProjectionMatrix()
	{
		Math3D::Mat4 perspective = Math3D::Mat4::GetPerspectiveProjection(fov, aspect, nearV, farV);
		perspective.yy *= -1;

		return perspective;
	}

	Math3D::Mat4 GetViewMatrix()
	{
		Math3D::Mat4 view = Math3D::Mat4::GetTranslateMatrix(position);

		return view;
	}

	Math3D::Mat4 GetLookAt()
	{
		Math3D::Mat4 view = Math3D::Mat4::GetLookAt(position, rotation, { 0.0f, 1.0f, 0.0f });

		return view;
	}

	void Update()
	{
		if (Input::IsDown(Input::A))
			position.x -= speed * (float)Clock::deltaTime;
		if (Input::IsDown(Input::D))
			position.x += speed * (float)Clock::deltaTime;

		if (Input::IsDown(Input::W))
			position.z -= speed * (float)Clock::deltaTime;
		if (Input::IsDown(Input::S))
			position.z += speed * (float)Clock::deltaTime;

		if (Input::IsDown(Input::Q))
			position.y -= speed * (float)Clock::deltaTime;
		if (Input::IsDown(Input::E))
			position.y += speed * (float)Clock::deltaTime;
	}
};

#endif