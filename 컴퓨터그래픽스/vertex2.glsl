#version 330 core
layout (location = 0) in vec3 vPos; //--- ���� ���α׷����� �޾ƿ� ���� ��ǥ��
layout (location = 1) in vec3 vColor; //--- ���� ���α׷����� �޾ƿ� ���� ��ǥ��
//uniform mat4 modelTransform; //--- �𵨸� ��ȯ ���: uniform ������ ����
uniform mat4 uRmatrix;
uniform mat4 scale;
uniform mat4 modelTransform;
uniform mat4 rotate;

void main()
{
//gl_Position = rotate * modelTransform * uRmatrix *scale* vec4(vPos, 1.0); //--- ��ǥ���� modelTransform ��ȯ�� �����Ѵ�.
gl_Position =  modelTransform * uRmatrix *scale* vec4(vPos, 1.0); //--- 15�� ����
//gl_Position =   uRmatrix *scale* vec4(vPos, 1.0); //--- 14�� ����
}
