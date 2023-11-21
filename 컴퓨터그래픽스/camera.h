#pragma once
#include "stdafx.h"

struct CAMERA {

	glm::mat4 camerapos = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 CR_matrix = glm::mat4(1.0f); //단위행렬로 초기화

	void Draw(glm::vec3 Camerapos, GLuint ID) {
		glm::vec3 cameraPos = { Camerapos }; //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0, 0, 0); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(ID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
		projection = projection * CR_matrix; //--- 공간을 z축 이동
		unsigned int projectionLocation = glGetUniformLocation(ID, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
};