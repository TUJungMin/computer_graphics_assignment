#version 330 core
in vec3 fColor;
out vec4 color;
void main (){
	color = vec4 (fColor, 1.0); //--- 빨강색으로 색상 고정
}