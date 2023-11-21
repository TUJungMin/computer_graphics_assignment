#include "stdafx.h"
#include "gameObject.h"

gameObject::gameObject() : m_modelVbo(-1), m_modelVertexNum(-1), m_position(glm::vec3(0, 0, 0)), m_rotate(glm::vec3(0, 0, 0)), m_scale(glm::vec3(1, 1, 1))
{
}

gameObject::gameObject(GLuint modelVBO, int modelVertexNum) : m_modelVbo(modelVBO), m_modelVertexNum(modelVertexNum), m_position(glm::vec3(0, 0, 0)),
m_rotate(glm::vec3(0, 0, 0)), m_scale(glm::vec3(1, 1, 1))
{
}

gameObject::gameObject(GLuint modelVBO, int moodelVertexNum, glm::vec3 position, glm::vec3 rootation, glm::vec3 scale)
	: m_modelVbo(modelVBO), m_modelVertexNum(moodelVertexNum), m_position(position),
	m_rotate(rootation), m_scale(scale)
{
}


void gameObject::Move(AXIS axis, float length)
{
	switch (axis)
	{
	case X:
	{
		m_position.x += length;
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

void gameObject::Rotate(AXIS axis, float angle)
{
	switch (axis)
	{
	case X:
	{
		m_rotate.x += angle;
	}
	break;
	case Y:
	{
		m_rotate.y += angle;
	}
	break;
	case Z:
	{
		m_rotate.z += angle;
	}
	break;
	default:
		break;
	}
}

void gameObject::SetScale(glm::vec3 scale)
{
	m_scale = scale;
}

void gameObject::SetScale(float x, float y, float z)
{
	m_scale.x = x;

	m_scale.y = y;

	m_scale.z = z;
}

void gameObject::SetColor(glm::vec3&& color)
{
	m_color = color;
}

void gameObject::Draw(GLuint shaderid, glm::mat4* paarentObject)
{
	glm::mat4 currentmatrix = glm::mat4(1.0f);
	if (nullptr != paarentObject)
	{
		currentmatrix = *paarentObject;
	}

	currentmatrix = glm::translate(currentmatrix, m_position);
	currentmatrix = glm::rotate(currentmatrix, glm::radians(m_rotate.y), AXIS_Y);
	currentmatrix = glm::rotate(currentmatrix, glm::radians(m_rotate.x), AXIS_X);
	currentmatrix = glm::rotate(currentmatrix, glm::radians(m_rotate.z), AXIS_Z);
	currentmatrix = glm::scale(currentmatrix, m_scale);

	/*for (auto& childobject : m_childGameObject) {
		childobject.Draw(shaderid, &currentmatrix);
	}*/

	glBindBuffer(GL_ARRAY_BUFFER, m_modelVbo);


	//unifom data mem -> gpu
	int matrixLoc = glGetUniformLocation(shaderid, "modelTransform");
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(currentmatrix));
	int colorLoc = glGetUniformLocation(shaderid, "fColor");
	glUniform3f(colorLoc, m_color.x, m_color.y, m_color.z);

	glDrawArrays(GL_TRIANGLES, 0, m_modelVertexNum);
}

