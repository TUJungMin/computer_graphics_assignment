#version 330 core
layout(location = 0) in vec3 vPos;
uniform vec3 uPos;

void main()
{	
	gl_Position = vec4(vPos + uPos, 1.0f);
}