#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

out vec3 FragPos; //--- 객체의 위치값을 프래그먼트 세이더로 보낸다.
out vec3 Normal; //--- 노멀값을 프래그먼트 세이더로 보낸다.

uniform mat4 modelTransform; //--- 모델링 변환값
uniform mat4 viewTransform; //--- 뷰잉 변환값
uniform mat4 projectionTransform; //--- 투영 변환값

void main()
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0);
	FragPos = vec3(modelTransform * vec4(vPos, 1.0));
	Normal = vec3(modelTransform * vec4(vNormal, 0.0));
}