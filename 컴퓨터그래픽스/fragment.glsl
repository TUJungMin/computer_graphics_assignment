#version 330 core
uniform vec3 uColor; //--- ���� ���α׷����� ���� �� ����
out vec4 FragColor; //--- ����� ��ü�� ����
void main()
{
   FragColor = vec4(uColor,1.0);
}
