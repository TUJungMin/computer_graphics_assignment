#pragma once
#include "../stdafx.h"
//� Ŭ������ ������� � �͵� ��Ŭ��� ���ϵ���... ��¿�� ���ٸ� �ؾ߰�����


class gameObject

{
	//ĳ���� ��ġ, ȸ�� ����
	glm::vec3	m_position;
	glm::vec3	m_rotate;
	glm::vec3	m_scale;
	glm::vec3	m_color;
	//�� ����
	GLuint		m_modelVbo;
	int			m_modelVertexNum;

public:
	gameObject();
	gameObject(GLuint modelVBO, int modelVertexNum);
	gameObject(GLuint modelVBO, int moodelVertexNum, glm::vec3 position, glm::vec3 rootation, glm::vec3 scale);
public:
	void Move(AXIS axis, float length); //������ �󸶸�ŭ �����̴���
	void Rotate(AXIS axis, float angle);//angle�� ��׸���
	void SetScale(glm::vec3 scale);
	void SetScale(float x, float y, float z);
public:
	void SetColor(glm::vec3&& color);
public:



public:
	void Draw(GLuint shaderid, glm::mat4* paarentObject);
	
};

