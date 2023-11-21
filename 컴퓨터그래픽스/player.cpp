#include "stdafx.h"
#include "player.h"


gameObject& player::AddChildObject(GLuint modelVBO, int moodelVertexNum, glm::vec3 position, glm::vec3 rootation, glm::vec3 scale)
{
	m_childGameObject.emplace_back(modelVBO, moodelVertexNum, position, rootation, scale);
	return m_childGameObject.back();
}

void player::Animate(float maximumAngle, int speed)
{


	// Update the current rotation angle based on the direction
	currentRotationAngle += rotationDirection * 1.0f;

	// Check if the rotation angle exceeds the maximum threshold
	if (std::abs(currentRotationAngle) > maximumAngle) {
		// Reverse the rotation direction
		rotationDirection = -rotationDirection;
	}
	m_childGameObject[3].Rotate(X, rotationDirection);
	m_childGameObject[4].Rotate(X, -rotationDirection);
	m_childGameObject[5].Rotate(X, -rotationDirection);
	m_childGameObject[6].Rotate(X, rotationDirection);
}

void player::Draw(GLuint shaderId)
{

	glm::mat4 currentmatrix = glm::mat4(1.0f);

	currentmatrix = glm::translate(currentmatrix, m_position);
	currentmatrix = glm::rotate(currentmatrix, glm::radians(m_rotate.y), AXIS_Y);
	currentmatrix = glm::rotate(currentmatrix, glm::radians(m_rotate.x), AXIS_X);
	currentmatrix = glm::rotate(currentmatrix, glm::radians(m_rotate.z), AXIS_Z);
	currentmatrix = glm::scale(currentmatrix, m_scale);

	for (auto& childobject : m_childGameObject) {
		childobject.Draw(shaderId, &currentmatrix);
	}
}


void player::Move(AXIS axis, float length)
{
	switch (axis)
	{
	case X:
	{
		m_position.x += length;
		if (m_position.x <= -0.5f) {
			m_childGameObject[0].Rotate(Y, 90);
		}
		else if (m_position.x >= 0.5f) {
			m_childGameObject[0].Rotate(Y, -90);
		}
	}
	break;
	case Y:
	{
		m_position.y += length;
	}
	break;
	case Z:
	{
		m_position.z += length;
	}
	break;
	default:
		break;
	}
}

void player::Rotate(AXIS axis, float angle)
{
	switch (axis)
	{
	case X:
	{
		m_rotate.x = angle;
	}
	break;
	case Y:
	{
		m_rotate.y = angle;
	}
	break;
	case Z:
	{
		m_rotate.z = angle;
	}
	break;
	default:
		break;
	}
}

void player::SetScale(glm::vec3 scale)
{
	m_scale = scale;
}

void player::SetScale(float x, float y, float z)
{
	m_scale.x = x;

	m_scale.y = y;

	m_scale.z = z;
}

void player::JUMP()
{
	if (!check_jump) {
		if (m_position.y < -0.1f)
		{
			m_position.y += 0.01f;
		}
		else
			check_jump = TRUE;
	}
	else
	{
		
		if (m_position.y > -0.3f)
		{
			m_position.y -= 0.01f;
		}
		

	}
}
