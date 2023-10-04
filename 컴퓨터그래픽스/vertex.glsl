#version 330 core
layout (location = 0) in vec3 vPos; //--- attribute�� ������ ��ġ �Ӽ�: �ε��� 0

uniform vec3 uPos;
vec3 rotate;
uniform float theta;
void main()
{

rotate.x = cos(theta*3.14/180)*vPos.x - sin(theta*3.14/180)*vPos.y;
rotate.y = sin(theta*3.14/180)*vPos.x + cos(theta*3.14/180)*vPos.y;
gl_Position = vec4 ((vPos + uPos),1.0f);			//9,10���� �ƴϸ� vPos�����

}
