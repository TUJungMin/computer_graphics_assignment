#pragma once
#include "stdafx.h"

struct CAMERA {

	glm::mat4 camerapos = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 CR_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ

	void Draw(glm::vec3 Camerapos, GLuint ID) {
		glm::vec3 cameraPos = { Camerapos }; //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0, 0, 0); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(ID, "viewTransform"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- ���� ���� ����: fovy, aspect, near, far
		projection = projection * CR_matrix; //--- ������ z�� �̵�
		unsigned int projectionLocation = glGetUniformLocation(ID, "projectionTransform"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
};