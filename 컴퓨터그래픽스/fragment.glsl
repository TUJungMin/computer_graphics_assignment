#version 330 core

uniform vec3 uColor;
out vec4 color;
void main ()
{
	color = vec4 (uColor, 1.0); //--- 빨강색으로 색상 고정
}