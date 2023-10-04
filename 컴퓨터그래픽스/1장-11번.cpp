#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;
char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
int draw_num = 0;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

GLvoid TimerFunc(int value);
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
vec3 Line_vertex[2] = { {-0.2f,0.0f,0.0f },{0.2f,0.0f,0.0f} };
vec3 Triangle_vertex[3] = {
	{-0.2f, -0.2f, 0.0f},  // 왼쪽 아래 꼭짓점
	{0.2f, -0.2f, 0.0f },   // 오른쪽 아래 꼭짓점
	{0.0f, 0.2f, 0.0f}     // 위 꼭짓점
};
vec3 Rect_vertex[4] = {
	{-0.2f,-0.2f,0.0f},
	{0.2f,-0.2f,0.0f},
	{0.0f,0.2f,0.0f},				//Rect_vertex[2],[3].x움직여야함
	{0.0f,0.2f,0.0f}
};
vec3 Pentagon_vertex[5] = {
	{-0.2f,0.2f,0.0f},
	{-0.2f,-0.2f,0.0f},
	{0.0f,0.2f,0.0f},				//Rect_vertex[2],[3].x움직여야함
	{0.2f,-0.2f,0.0f},
	{0.2f,0.2f,0.0f}
};
vec3 point_vertex[5] = {
	{-0.3f,0.2f,0.0f},
	{-0.2f,-0.2f,0.0f},
	{0.0f,0.4f,0.0f},				//Rect_vertex[2],[3].x움직여야함
	{0.2f,-0.2f,0.0f},
	{0.3f,0.2f,0.0f}
};
vec3 raw[2] = { { -1,0,0 }, { 1,0,0 } };
vec3 col[2] = { { 0,-1,0 }, { 0,1,0 } };
GLvoid Timer(int value);
bool first_press = FALSE;
bool button_l = FALSE, button_a = FALSE, button_t = FALSE, button_p = FALSE, button_r = FALSE;
struct Sector {
	vec3 move_coord;
	GLuint m_vbo;

	void InitVbo(vec3* a) {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), a, GL_STATIC_DRAW);
	}
	void Draw() {

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, 0, 0, 1.0f);
		int UvertexUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(UvertexUniformLoc, 0, 0, 0);
		int scaleUniformLoc = glGetUniformLocation(shaderProgramID, "scale");
		glUniform3f(scaleUniformLoc, 1, 1, 0);

		glLineWidth(5.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}

};
struct Line {

	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	vec3 move_Pos;
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line_vertex), Line_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);		//wasd로 움직이는 거리+초기좌표
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}
};
struct Rect {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	vec3 move_Pos;
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_vertex), Rect_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);		//wasd로 움직이는 거리+초기좌표
		glLineWidth(10.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
	}
	void triangle_to_rect()
	{
		if (Rect_vertex[2].x > -0.2) {
			Rect_vertex[2].x -= 0.004f;
		}
		if (Rect_vertex[3].x < 0.2) {
			Rect_vertex[3].x += 0.004f;
		}
		else
		{
			move_coord.x = 0.5;
			move_coord.y = 0.5;
			Rect_vertex[2].x = 0;
			Rect_vertex[3].x = 0;
			button_t = FALSE;
			button_a = FALSE;
			first_press = FALSE;
		}
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_vertex), Rect_vertex, GL_STATIC_DRAW);
	}
};
struct Triangle {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	vec3 move_Pos;
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_vertex), Triangle_vertex, GL_STATIC_DRAW);
	}

	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);

		glLineWidth(5.0f);
		if(!button_l)
		glDrawArrays(GL_LINES, 0, 2);
		else
			glDrawArrays(GL_TRIANGLES, 0, 3);
		
	}

	void line_to_triangle()
	{
		if (Triangle_vertex[2].y < 0.2) {
			Triangle_vertex[2].y += 0.008f;
		}
		else
		{
			Triangle_vertex[2].y = 0.0f;
			button_l = FALSE;
			button_a = FALSE;
			first_press = FALSE;
			move_coord.x = -0.5f;
			move_coord.y =  0.5f;
		}
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_vertex), Triangle_vertex, GL_STATIC_DRAW);
		
	}


};
struct Pentagon {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	vec3 move_Pos;
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
		glDrawArrays(GL_TRIANGLES, 2, 3);
	}
	void rect_to_pentagon()
	{
		 if (Pentagon_vertex[2].y < 0.4)
		{
			Pentagon_vertex[2].y += 0.004f;
			Pentagon_vertex[0].x -= 0.002f;
			Pentagon_vertex[4].x += 0.002f;
		}
		else
		{
			 Pentagon_vertex[2].y = 0.2f;
			 Pentagon_vertex[0].x = -0.2f;
			 Pentagon_vertex[4].x = 0.2f;
			 first_press = FALSE;
			button_r = FALSE;
			button_a = FALSE;
			move_coord.x = -0.5f;
			move_coord.y = -0.5f;
		}
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
	}
};
struct Point {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	vec3 move_Pos;
	vec3 first_coord[5];
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertex), point_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
		glDrawArrays(GL_TRIANGLES, 2, 3);
	}
	void pentagon_to_point()
	{
		for (auto& a : point_vertex)
		{
			if (a.x != 0 && a.x>0)
				a.x -= 0.01f;
			if (a.x != 0 && a.x < 0)
				a.x += 0.01f;

			if (a.y != 0 && a.y > 0)
				a.y -= 0.01f;
			if (a.y != 0 && a.y < 0)
				a.y += 0.01f;
		}
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertex), point_vertex, GL_STATIC_DRAW);

	}
};

Sector RAW;
Sector COL;
Triangle g_TriangleShape;
Rect g_rect;
Pentagon g_pentagon;
Point g_point;
void initialize_triangle(){	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);	g_TriangleShape.color.r = urd(dre);	g_TriangleShape.color.g = urd(dre);	g_TriangleShape.color.b = urd(dre);	g_TriangleShape.move_coord.x = -0.5f;	g_TriangleShape.move_coord.y = 0.5f;	g_TriangleShape.InitVbo();}void initialize_rect(){	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);	g_rect.color.r = urd(dre);	g_rect.color.g = urd(dre);	g_rect.color.b = urd(dre);	g_rect.move_coord.x = 0.5f;	g_rect.move_coord.y = 0.5f;	g_rect.InitVbo();}void initialize_pentagon()
{
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);	g_pentagon.color.r = urd(dre);	g_pentagon.color.g = urd(dre);	g_pentagon.color.b = urd(dre);	g_pentagon.move_coord.x = -0.5f;	g_pentagon.move_coord.y = -0.5f;	g_pentagon.InitVbo();
}
void initialize_point()
{
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);	g_point.color.r = urd(dre);	g_point.color.g = urd(dre);	g_point.color.b = urd(dre);	g_point.move_coord.x = 0.5f;	g_point.move_coord.y = -0.5f;	for (int i = 0; i < 5; i++) {		g_point.first_coord[i] = point_vertex[i];
	}
	g_point.InitVbo();
}

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(200, 200); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("OPENGL"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized" << endl;
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	RAW.InitVbo(raw);
	COL.InitVbo(col);
	initialize_triangle();
	initialize_rect();
	initialize_pentagon();
	initialize_point();
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutTimerFunc(20, Timer, 1);
	glutKeyboardFunc(KeyBoardFunc);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene()
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);
	if(!first_press||button_a){
	RAW.Draw();	COL.Draw();
	}
	if (button_l || !first_press) {
		g_TriangleShape.Draw();
	}
	if (button_t || !first_press) {
		g_rect.Draw();
	}
	if (button_r || !first_press) {
		g_pentagon.Draw();
	}
	if (button_p || !first_press) {
		g_point.Draw();
	}
	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
GLvoid Timer(int value)
{
	if (button_l&&first_press) {
		g_TriangleShape.line_to_triangle();
	}

	if (button_t && first_press) {
		g_rect.triangle_to_rect();
	}
	if (button_r && first_press) {
		g_pentagon.rect_to_pentagon();
	}
	if (button_p && first_press) {
		g_point.pentagon_to_point();
	}
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 1);
}


GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	first_press = TRUE;
	switch (key)
	{
	case 'l':
		Triangle_vertex[2].y = -0.2f;
		button_l = TRUE;
		button_t = FALSE;
		button_r = FALSE;
		button_p = FALSE;
		button_a = FALSE;
		g_TriangleShape.move_coord.x = 0;
		g_TriangleShape.move_coord.y = 0;
		
		break;
	case 't':
		Rect_vertex[2].x = 0;
		Rect_vertex[3].x = 0;
		button_l = FALSE;
		button_t = TRUE;
		button_r = FALSE;
		button_p = FALSE;
		button_a = FALSE;
		g_rect.move_coord.x = 0;
		g_rect.move_coord.y = 0;
		break;
	case 'r':
		Pentagon_vertex[0].x = -0.2f;
		Pentagon_vertex[4].x = 0.2f;
		Pentagon_vertex[2].y = 0.2f;
		button_r = TRUE;
		button_l = FALSE;
		button_t = FALSE;
		button_p = FALSE;
		button_a = FALSE;
		g_pentagon.move_coord.x = 0;
		g_pentagon.move_coord.y = 0;
		break;
	case 'p':
		for (int i = 0; i < 5; i++) {
			point_vertex[i] = g_point.first_coord[i];
		}
		button_p = TRUE;
		button_l = FALSE;
		button_t = FALSE;
		button_r = FALSE;
		button_a = FALSE;
		g_point.move_coord.x = 0;
		g_point.move_coord.y = 0;
		break;

	case 'a':
		Triangle_vertex[2].y = -0.2f;
		Rect_vertex[2].x = 0;
		Rect_vertex[3].x = 0;
		Pentagon_vertex[0].x = -0.2f;
		Pentagon_vertex[4].x = 0.2f;
		Pentagon_vertex[2].y = 0.2f;
		for (int i = 0; i < 5; i++) {
			point_vertex[i] = g_point.first_coord[i];
		}
		button_l = TRUE;
		button_t = TRUE;
		button_r = TRUE;
		button_p = TRUE;
		button_a = TRUE;;
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

void make_vertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex.glsl");

	//vertex shader 생성 해주고 생성한 데이터를 vertexShader라는 변수에 저장
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//성공하면 0이 아닌값

	//세이더 객체, 1, 세이더 소스 코드, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, 문자열들의 갯수, 문자열들의 배열, 문자열들의 길이를 저장할 int배열

	//컴파일 해야지요.
	glCompileShader(vertexShader);

	//세이더 결과 어떻게 됐는지? 상태 확인하는 코드
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
GLuint make_shaderProgram() {
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기

	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}