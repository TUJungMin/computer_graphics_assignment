#version 330 core
layout (location = 0) in vec3 vPos; //--- 응용 프로그램에서 받아온 도형 좌표값
layout (location = 1) in vec3 vColor; //--- 응용 프로그램에서 받아온 도형 좌표값
//uniform mat4 modelTransform; //--- 모델링 변환 행렬: uniform 변수로 선언
uniform mat4 uRmatrix;
uniform mat4 scale;
uniform mat4 modelTransform;
uniform mat4 rotate;

void main()
{
//gl_Position = rotate * modelTransform * uRmatrix *scale* vec4(vPos, 1.0); //--- 좌표값에 modelTransform 변환을 적용한다.
gl_Position =  modelTransform * uRmatrix *scale* vec4(vPos, 1.0); //--- 15번 쓸거
//gl_Position =   uRmatrix *scale* vec4(vPos, 1.0); //--- 14번 쓸거
}
