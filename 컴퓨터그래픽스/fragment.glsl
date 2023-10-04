#version 330 core
uniform vec3 uColor; //--- 응용 프로그램에서 변수 값 설정
out vec4 FragColor; //--- 출력할 객체의 색상
void main()
{
   FragColor = vec4(uColor,1.0);
}
