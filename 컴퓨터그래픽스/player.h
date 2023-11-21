#pragma once
#include "gameObject/gameObject.h"

class player
{
public:
	glm::vec3	m_position;
	glm::vec3	m_rotate;
	glm::vec3	m_scale;
	glm::vec3	m_color;
	bool check_jump = FALSE;
	
private:
	std::vector<gameObject> m_childGameObject;
public:
	player() :m_position(glm::vec3(0, -0.3f, 0)), m_rotate(glm::vec3(0, 0, 0)), m_scale(glm::vec3(1, 1, 1)), currentRotationAngle(0), rotationDirection(1) , speed(1)
	{}
public:
	gameObject& AddChildObject(GLuint modelVBO, int moodelVertexNum, glm::vec3 position, glm::vec3 rootation, glm::vec3 scale);
	void Animate(float maximumAngle,int speed);
	void Draw(GLuint shaderId);

	void Move(AXIS axis, float length); //������ �󸶸�ŭ �����̴���
	void Rotate(AXIS axis, float angle);//angle�� ��׸���
	void SetScale(glm::vec3 scale);
	void SetScale(float x, float y, float z);
	void JUMP();
private:
	float currentRotationAngle;  // ���� ȸ�� ������ ����
	int rotationDirection;       // ȸ�� ���� (1 �Ǵ� -1)
	int speed;       // ȸ�� ���� (1 �Ǵ� -1)
};
