#pragma once
#include "../stdafx.h"
//어떤 클래스의 헤더에는 어떤 것도 인클루드 안하도록... 어쩔수 없다면 해야겠지만


class gameObject

{
	//캐릭터 위치, 회전 정도
	glm::vec3	m_position;
	glm::vec3	m_rotate;
	glm::vec3	m_scale;
	glm::vec3	m_color;
	//모델 정보
	GLuint		m_modelVbo;
	int			m_modelVertexNum;

public:
	gameObject();
	gameObject(GLuint modelVBO, int modelVertexNum);
	gameObject(GLuint modelVBO, int moodelVertexNum, glm::vec3 position, glm::vec3 rootation, glm::vec3 scale);
public:
	void Move(AXIS axis, float length); //축으로 얼마만큼 움직이는지
	void Rotate(AXIS axis, float angle);//angle은 디그리값
	void SetScale(glm::vec3 scale);
	void SetScale(float x, float y, float z);
public:
	void SetColor(glm::vec3&& color);
public:



public:
	void Draw(GLuint shaderid, glm::mat4* paarentObject);
	
};

